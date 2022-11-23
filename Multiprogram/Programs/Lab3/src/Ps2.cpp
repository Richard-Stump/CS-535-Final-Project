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

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color
) {
    printf("        Draw TRI\n");
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