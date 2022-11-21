#include "Ps2.hpp"

#include <stdio.h>
#include <tamtypes.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>

#include <kernel.h>
#include <malloc.h>
#include <tamtypes.h>
#include <gs_psm.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>
#include <packet2_vif.h>
#include <packet2_types.h>
#include <graph.h>
#include <draw.h>

#include <vector>

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Misc.hpp"
#if USE_SDL2 
	#include <SDL2/SDL.h>
	#include "UseSdl.hpp"
#endif//USE_SDL2

extern "C" {
    extern u32 VU1DrawLines_CodeStart __attribute__((section(".vudata")));
    extern u32 VU1DrawLines_CodeEnd __attribute__((section(".vudata")));
}

namespace globals
{
	framebuffer_t 	framebuffer;
	zbuffer_t 		zbuffer;

	packet2_t* 		vu1Packets[2] __attribute__((aligned(64)));
	packet2_t*		curVu1Packet __attribute__((aligned(64)));
	u32				curVu1PacketIdx;

	packet2_t* 		gifPacket;

	packet2_t* 		gsPacket;

	prim_t 			primitive;
}

void uploadProgram()
{
    printf("Uplaoding microprogram to vu1");

    u32 packet_size =
		packet2_utils_get_packet_size_for_program(&VU1DrawLines_CodeStart, &VU1DrawLines_CodeEnd) + 1; // + 1 for end tag
	packet2_t *packet2 = packet2_create(packet_size, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_vif_add_micro_program(packet2, 0, &VU1DrawLines_CodeStart, &VU1DrawLines_CodeEnd);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);

    printf("        Done!\n");
}

/** Some initialization of GS and VRAM allocation */
void init_gs(framebuffer_t *t_frame, zbuffer_t *t_z)
{
	// Define a 32-bit 640x512 framebuffer.
	t_frame->width = 640;
	t_frame->height = 512;
	t_frame->mask = 0;
	t_frame->psm = GS_PSM_32;
	t_frame->address = graph_vram_allocate(t_frame->width, t_frame->height, t_frame->psm, GRAPH_ALIGN_PAGE);

	// Enable the zbuffer.
	t_z->enable = DRAW_ENABLE;
	t_z->mask = 0;
	t_z->method = ZTEST_METHOD_GREATER_EQUAL;
	t_z->zsm = GS_ZBUF_32;
	t_z->address = graph_vram_allocate(t_frame->width, t_frame->height, t_z->zsm, GRAPH_ALIGN_PAGE);

	// Initialize the screen and tie the first framebuffer to the read circuits.
	graph_initialize(t_frame->address, t_frame->width, t_frame->height, t_frame->psm, 0, 0);
}

/** Some initialization of GS 2 */
void init_drawing_environment(framebuffer_t *t_frame, zbuffer_t *t_z)
{
	packet2_t *packet2 = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// This will setup a default drawing environment.
	packet2_update(packet2, draw_setup_environment(packet2->next, 0, t_frame, t_z));

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	packet2_update(packet2, draw_primitive_xyoffset(packet2->next, 0, (2048 - 320), (2048 - 256)));

	// Finish setting up the environment.
	packet2_update(packet2, draw_finish(packet2->next));

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();

	packet2_free(packet2);
}

void vu1_set_double_buffer_settings()
{
	packet2_t *packet2 = packet2_create(1, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_utils_vu_add_double_buffer(packet2, 0, 512);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

void initPrimitive()
{
	using globals::primitive;

	primitive.type = PRIM_TRIANGLE;
	primitive.shading = PRIM_SHADE_FLAT;
	primitive.mapping = DRAW_DISABLE;
	primitive.fogging = DRAW_DISABLE;
	primitive.antialiasing = DRAW_DISABLE;
	primitive.colorfix = PRIM_UNFIXED;
	primitive.blending = DRAW_DISABLE;
	primitive.mapping_type = PRIM_MAP_ST;
}

/** Send packet which will clear our screen. */
void clear_screen(framebuffer_t *frame, zbuffer_t *z)
{
    graph_wait_vsync();

	packet2_t *clear = packet2_create(35, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// Clear framebuffer but don't update zbuffer.
	packet2_update(clear, draw_disable_tests(clear->next, 0, z));
	packet2_update(clear, draw_clear(clear->next, 0, 2048.0f - 320.0f, 2048.0f - 256.0f, frame->width, frame->height, 0x10, 0x10, 0x10));
	packet2_update(clear, draw_enable_tests(clear->next, 0, z));
	packet2_update(clear, draw_finish(clear->next));

	// Now send our current dma chain.
	dma_wait_fast();
	dma_channel_send_packet2(clear, DMA_CHANNEL_GIF, 1);

	packet2_free(clear);

	// Wait for scene to finish drawing
	draw_wait_finish();
}

void ps2ClearScreen()
{
#if USE_SDL2
	SdlClearScreen();
#else
	clear_screen(&globals::framebuffer, &globals::zbuffer);
#endif//USE_SDL2
}

void swapPackets() {
	globals::curVu1PacketIdx = globals::curVu1PacketIdx == 0 ? 1 : 0;
	globals::curVu1Packet = globals::vu1Packets[globals::curVu1PacketIdx];
}

void calculateGifPacket(s32 vertCount, MATRIX matrix) 
{
	using globals::gifPacket;
	using globals::primitive;

	packet2_reset(gifPacket, 0);

	packet2_add_float(gifPacket, 2048.0f);
	packet2_add_float(gifPacket, 2048.0f);
	packet2_add_float(gifPacket, ((float)0xFFFFFF) / 32.0F);
	packet2_add_s32(gifPacket, vertCount);

	packet2_add_u32(gifPacket, 0xFFFFFFFF);
	packet2_add_u32(gifPacket, 0xFFFFFFFF);
	packet2_add_u32(gifPacket, 0xFFFFFFFF);
	packet2_add_u32(gifPacket, 0xFFFFFFFF);
	
	packet2_add_float(gifPacket, matrix[0]);
	packet2_add_float(gifPacket, matrix[1]);
	packet2_add_float(gifPacket, matrix[2]);
	packet2_add_float(gifPacket, matrix[3]);

	packet2_add_float(gifPacket, matrix[4]);
	packet2_add_float(gifPacket, matrix[5]);
	packet2_add_float(gifPacket, matrix[6]);
	packet2_add_float(gifPacket, matrix[7]);
	
	packet2_add_float(gifPacket, matrix[8]);
	packet2_add_float(gifPacket, matrix[9]);
	packet2_add_float(gifPacket, matrix[10]);
	packet2_add_float(gifPacket, matrix[11]);

	packet2_add_float(gifPacket, matrix[12]);
	packet2_add_float(gifPacket, matrix[13]);
	packet2_add_float(gifPacket, matrix[14]);
	packet2_add_float(gifPacket, matrix[15]);

	packet2_utils_gif_add_set(gifPacket, 1);
	packet2_utils_gs_add_prim_giftag(gifPacket, &primitive, vertCount, DRAW_RGBAQ_REGLIST, 2, 0);
}

void drawTrianglesWireframeVu1(
    std::vector<glm::vec4> verts, 
    glm::mat4& matTrans,
    glm::vec4 color
)
{
	using globals::gifPacket;

	static MATRIX localMatrix;

	packet2_t* packet __attribute__((aligned(64))) = globals::curVu1Packet;
	if(packet == nullptr) {
		printf("PACKET IS NULL!\n");
	}

	float* matData = glm::value_ptr(matTrans);
	for(int i = 0; i < 16; i++) 
		localMatrix[i] = matData[i];

	packet2_reset(packet, 0);

	// Construct a gif packet that vu1 will use to send the transformed geometry
	// to the GS
	s32 vert_count = verts.size();
	calculateGifPacket(vert_count, localMatrix);
	u32 gifSize = packet2_get_qw_count(gifPacket);

	u32 addedQwords = 0;
	packet2_utils_vu_add_unpack_data(packet, addedQwords, gifPacket->base, gifSize, 1);
	addedQwords += gifSize;

	packet2_utils_vu_add_unpack_data(packet, addedQwords, (void*)verts.data(), vert_count, 1);
	addedQwords += vert_count;

	packet2_utils_vu_add_start_program(packet, 0);
	packet2_utils_vu_add_end_tag(packet);

	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	dma_channel_send_packet2(packet, DMA_CHANNEL_VIF1, 1);

	// Swap the current vu1 packet
	swapPackets();

	printf("Swapped packets. \n     vcount = %d\n"
		   "    gifSize = %d\n     totsize = %d\n"
		   "    addedQwords = %d\n",
	vert_count, gifSize, packet2_get_qw_count(packet),
	addedQwords

	);
}

void drawTrianglesWireframeEE(
    std::vector<glm::vec4> verts, 
    glm::mat4& matTrans,
    glm::vec4 color
)
{
	using globals::primitive;

	packet2_t* packet = globals::gsPacket;

	packet2_reset(packet, 0);
	packet2_utils_gif_add_set(packet, 0);
	packet2_utils_gs_add_prim_giftag(packet, &primitive, verts.size(), DRAW_RGBAQ_REGLIST, 2, 0);

	for(size_t i = 0; i < verts.size(); i++) {
		printf("     Adding vertex %d\n", i);

		glm::vec4 vertex = verts[i];

		// Transform the vertex
		vertex = vertex * matTrans;	// Normal transformation matrix
		vertex = vertex / vertex.w;	// Perspective divide

		// Transform the vertex into the GS viewport
		vertex.x = (1.0f + vertex.x) * 2048.0f;
		vertex.y = (1.0f + vertex.y) * 2048.0f;
		vertex.z = (1.0f + vertex.z) * ((float)0xFFFFFF) / 32.0f;

		// Add the vertex to the draw list
		xyz_t gsPos;
		gsPos.x = ftoi4(vertex.x);
		gsPos.y = ftoi4(vertex.y);
		gsPos.z = ftoi4(vertex.z);

		color_t gsColor;
		gsColor.r = 255;
		gsColor.g = 255;
		gsColor.b = 255;
		gsColor.a = 255;
		gsColor.q = 1.0f;
		
		packet2_add_2x_s64(packet, gsColor.rgbaq, gsPos.xyz);
	}

	// Kick the vertices to the GS
	dma_channel_send_packet2(packet, DMA_CHANNEL_GIF, 1);
	dma_channel_wait(DMA_CHANNEL_GIF, 0);

	printf("Dma Packet Sent\n");
}

void ps2DrawTrianglesWireframe(
    std::vector<glm::vec4> verts, 
    glm::mat4& matTrans,
    glm::vec4 color)
{
#if USE_SDL2
	drawTrianglesSDL(verts, matTrans, color);
#else
	drawTrianglesWireframeEE(verts, matTrans, color);
#endif
}

void initPs2()
{
#if USE_SDL2
	initSdl();
#else
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

	globals::gsPacket = packet2_create(1024, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	globals::vu1Packets[0] = packet2_create(512, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	globals::vu1Packets[1] = packet2_create(512, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	globals::curVu1PacketIdx = 0;
	globals::curVu1Packet = globals::vu1Packets[globals::curVu1PacketIdx];
	globals::gifPacket = packet2_create(8, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

    uploadProgram();
	vu1_set_double_buffer_settings();

	init_gs(&globals::framebuffer, &globals::zbuffer);
	init_drawing_environment(&globals::framebuffer, &globals::zbuffer);
	initPrimitive();
#endif//USE_SDL2
}

