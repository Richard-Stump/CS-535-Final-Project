#include "Ps2.hpp"

#include <stdio.h>
#include <iostream>

#include <kernel.h>
#include <malloc.h>
#include <tamtypes.h>
#include <gs_psm.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>
#include <graph.h>
#include <draw.h>
#include <vif_codes.h>

framebuffer_t	framebuffer;
zbuffer_t		zbuffer;

namespace ps2 {


void initGs() 
{
	std::cout << "    Allocating framebuffer" << std::endl;
	framebuffer.width = 640;
	framebuffer.height = 512;
	framebuffer.mask = 0;
	framebuffer.psm = GS_PSM_32;
	framebuffer.address = graph_vram_allocate(
		framebuffer.width,
		framebuffer.height,
		framebuffer.psm,
		GRAPH_ALIGN_PAGE
	);

	std::cout << "    Allocating z buffer" << std::endl;
	zbuffer.enable 	= DRAW_ENABLE;
	zbuffer.mask	= 0;
	zbuffer.method	= ZTEST_METHOD_GREATER_EQUAL;
	zbuffer.zsm		= GS_ZBUF_32;
	zbuffer.address = graph_vram_allocate(
		framebuffer.width,
		framebuffer.height,
		zbuffer.zsm,
		GRAPH_ALIGN_PAGE
	);

	std::cout << "    graph_initialize()" << std::endl;
	graph_initialize(
		framebuffer.address,
		framebuffer.width,
		framebuffer.height,
		framebuffer.psm,
		0,
		0
	);

}

void initDrawingEnvironment()
{
	packet2_t* packet = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	packet2_update(
		packet,
		draw_setup_environment(packet->next, 0, &framebuffer, &zbuffer)
	);
	packet2_update(
		packet,
		draw_primitive_xyoffset(packet->next, 0, (2048 - 320), (2048 - 256))
	);

	packet2_update(packet, draw_finish(packet->next));

	dma_wait_fast();
	dma_channel_send_packet2(packet, DMA_CHANNEL_GIF, 1);

	packet2_free(packet);
}

void clearScreen()
{

}

bool init()
{
	std::cout << "    Drawing environment: PS2 path2" << std::endl;

	// Init DMA channels.
	dma_channel_initialize(DMA_CHANNEL_GIF, nullptr, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

	// Init the GS, framebuffer, zbuffer, and texture buffer.
	initGs();

	// Init the drawing environment and framebuffer.
	initDrawingEnvironment();

	std::cout << "    Done!" << std::endl;
}


void close()
{

}

// Called at the start of the frame. This does things like clear the screen,
// and other stuff.
void startFrame()
{
	std::cout << "Start of frame" << std::endl;
	clearScreen();
}

// Called at the end of the frame. This does things like
void endFrame()
{
	std::cout << "    End of frame" << std::endl;
	graph_wait_vsync();
}

inline void addPrimitive(packet2_t* packet)
{
	prim_t prim;
	prim.type = PRIM_LINE_STRIP;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = false;
	prim.fogging = false;
	prim.blending = false;
	prim.antialiasing = false;
	prim.mapping_type = PRIM_MAP_ST;
	prim.colorfix = PRIM_UNFIXED;

	// Add a new primitive tag to the packet. Since we are rendering in line
	// 
	packet2_utils_gs_add_prim_giftag(
		packet,
		&prim,
		1,								// We only have one loop
		((GIF_REG_RGBAQ & 0xF) << 0) |
		((GIF_REG_XYZ2 & 0xF) << 4),
		8,								// 4 vertices * 2 registers (rgba, xyz2)
		0	
	);
}

inline void addVertex(packet2_t* packet, glm::vec4 pos, glm::vec4 color)
{
	packet2_add_u32(packet, (u8)(color.r * 255.0f));
	packet2_add_u32(packet, (u8)(color.g * 255.0f));
	packet2_add_u32(packet, (u8)(color.b * 255.0f));
	packet2_add_u32(packet, (u8)(color.a * 255.0f));

	packet2_add_u32(packet, ftoi4(pos.x));
	packet2_add_u32(packet, ftoi4(pos.y));
	packet2_add_u32(packet, ftoi4(pos.z));
	packet2_add_u32(packet, ftoi4(pos.w));
}

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color
){
	std::cout << "    Creating packet to send model to GS" << std::endl;

	// Open a packet in direct mode since we are rendering through path2
	packet2_t* packet = packet2_create(512, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_pad96(packet, 0);
	packet2_vif_open_direct(packet, 0);

	std::cout << "    Adding the vertex data" << std::endl;
	// Now we build our gif tags containing the vertex data.
	for(int i = 0; i < verts.size(); i += 3) {
		glm::vec4 v1 = verts[i];
		glm::vec4 v2 = verts[i + 1];
		glm::vec4 v3 = verts[i + 2];

		// Add then to our packet. v1 gets added to the end since we are trying
		// to build a line-loop
		addPrimitive(packet);
		addVertex(packet, v1, color);
		addVertex(packet, v2, color);
		addVertex(packet, v3, color);
		addVertex(packet, v1, color);
	}


	// We are done adding our gif tags, so we have to close the direct mode
	// before doing anything else with the packet. This automatically counts
	// the size of the direct data for us.
	std::cout << "    Closing the VIF direct mode" << std::endl;
	packet2_vif_close_direct_auto(packet);

	// Send it
	std::cout << "    Waiting on VIF" << std::endl;
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	std::cout << "    Sending" << std::endl;
	dma_channel_send_packet2(packet, DMA_CHANNEL_VIF1, 1);
	std::cout << "    Waiting on VIF" << std::endl;
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);

	std::cout << "    Done" << std::endl;
	packet2_free(packet);
}

} // namespace ps2