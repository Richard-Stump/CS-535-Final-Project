#include "Ps2.hpp"

#include <stdio.h>

#include <kernel.h>
#include <tamtypes.h>
#include <gs_psm.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>
#include <graph.h>
#include <draw.h>

static framebuffer_t    framebuffer;
static zbuffer_t        zbuffer;

namespace ps2::draw {

void clearScreen()
{
    packet2_t* packet = packet2_create(35, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

    packet2_update(packet, draw_disable_tests(packet->next, 0, &zbuffer));
    packet2_update(
        packet, 
        draw_clear(
            packet->next, 
            0, 
            2048.0f - 320.0f,       2048.0f - 256.0f, 
            framebuffer.width,      framebuffer.height,
            128, 128, 128
        )
    );

    packet2_update(packet, draw_enable_tests(packet->next, 0, &zbuffer));
    packet2_update(packet, draw_finish(packet->next));

    dma_wait_fast();
    dma_channel_send_packet2(packet, DMA_CHANNEL_GIF, 1);

    packet2_free(packet);

    draw_wait_finish();
}

void beginFrame()
{
    printf("Begin frame\n");
    clearScreen();
}

void endFrame()
{
    graph_wait_vsync();
}

inline void addPrimitive(packet2_t* packet)
{
    // Add the tag for the primitive
    packet2_add_u64(packet,
        GIF_SET_TAG(
            3,                          // Number of loop
            1,                          // End of packet
            1,                          // Primitive Enable
            GIF_SET_PRIM(          
                GIF_PRIM_TRIANGLE,      // Primitive Enable
                1,                      // Shading: 0 = flat, 1 = goroud
                0,                      // Texture mapping enable
                0,                      // Fog enable
                0,                      // Alpha blend enable
                0,                      // Antialias enable
                0,                      // Texture Map type: 0=stq, 1=uv
                0,                      // Context number
                0                       // Fix fragment value
            ),
            GIF_FLG_PACKED,             // Data format
            2                           // Number of registers per loop
        )
    );

    packet2_add_u64(
        packet,
        (GIF_REG_RGBAQ & 0xF) << 0 |
        (GIF_REG_XYZ2 & 0xF) << 4
    );  
}

inline void addVertex(packet2_t* packet, glm::vec4 pos, glm::vec4 color)
{
    packet2_add_u32(packet, 255);   // R
    packet2_add_u32(packet, 0);     // G
    packet2_add_u32(packet, 0);     // B
    packet2_add_u32(packet, 255);   // A

    packet2_add_u32(packet, ftoi4(pos.x));
    packet2_add_u32(packet, ftoi4(pos.y));
    packet2_add_u32(packet, ftoi4(pos.z));
    packet2_add_u32(packet, 0x00);
}

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color
) {
    printf("        Draw TRI\n");

    packet2_t* packet = packet2_create(512, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

    // Start the packet in direct mode. this is necessary since we are rending
    // over path2.
    packet2_pad96(packet, 0);
    packet2_vif_flush(packet, 0);
    packet2_pad96(packet, 0);
    packet2_vif_open_direct(packet, 0);
    
    addPrimitive(packet);
    addVertex(packet, glm::vec4{-1024.0f, 0.0f, -1.0f, 0.0f}, glm::vec4());
    addVertex(packet, glm::vec4{ 1024.0f, 0.0f, -1.0f, 0.0f}, glm::vec4());
    addVertex(packet, glm::vec4{ 0.0f, 1024.0f, -1.0f, 0.0f}, glm::vec4());

    // Close the direct mode. This will automatically count the number of qwords
    // added for us and construct the VIF command to pass the data to the GIF
    packet2_vif_close_direct_auto(packet);
    
    packet2_utils_vu_add_end_tag(packet);

    printf("                    Waiting on DMA\n");
    dma_channel_wait(DMA_CHANNEL_GIF, 0);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
    printf("                    Sending Tri\n");
    dma_channel_send_packet2(packet, DMA_CHANNEL_VIF1, 1);

    packet2_free(packet);
}
//==============================================================================
// Initialization code
//==============================================================================

void initGs(void)
{
    printf("    Allocating framebuffer\n");
    framebuffer.width   = 640;
    framebuffer.height  = 512;
    framebuffer.mask    = 0;
    framebuffer.psm     = GS_PSM_32;
    framebuffer.address = graph_vram_allocate(
        framebuffer.width,
        framebuffer.height,
        framebuffer.psm,
        GRAPH_ALIGN_PAGE
    );

    printf("    Allocating z buffer\n");
    zbuffer.enable  = DRAW_ENABLE;
    zbuffer.mask    = 0;
    zbuffer.method  = ZTEST_METHOD_GREATER_EQUAL;
    zbuffer.zsm     = GS_ZBUF_32;
    zbuffer.address = graph_vram_allocate(
        framebuffer.width, 
        framebuffer.height,
        zbuffer.zsm,
        GRAPH_ALIGN_PAGE
    );

    printf("    Setting GS to use the framebuffer\n");
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

    printf("    Setting up drawing environment\n");
    packet2_update(
        packet,
        draw_setup_environment(packet->next, 0, &framebuffer, &zbuffer)
    );

    printf("    Setting primitive offset\n");
    packet2_update(
        packet,
        draw_primitive_xyoffset(packet->next, 0, (2048-320), (2048-256))
    );

    printf("    Adding finish to packet\n");
    packet2_update(
        packet,
        draw_finish(packet->next)
    );

    printf("    Sending the packet\n");
    dma_channel_send_packet2(packet, DMA_CHANNEL_GIF, 1);

    printf("    Waiting on DMA\n");
    dma_wait_fast();

    printf("        Done\n");
    packet2_free(packet);
}

bool init()
{
    printf("    Drawing environment: path2\n");

    initGs();
    initDrawingEnvironment();

    return true;
}

void close()
{

}


} // namespace ps2::draw