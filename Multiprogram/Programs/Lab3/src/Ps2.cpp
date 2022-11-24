#include "Ps2.hpp"

#include <stdio.h>
#include <fstream>

#include <kernel.h>
#include <tamtypes.h>
#include <gs_psm.h>
#include <dma.h>
#include <packet2.h>
#include <packet2_utils.h>
#include <graph.h>
#include <draw.h>
#include <gif_tags.h>
#include <vif_codes.h>

static framebuffer_t    framebuffer;
static zbuffer_t        zbuffer;

packet2_t*  packets[2];
int         context = 0;

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
            32, 32, 32
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
    prim_t primitive;
    primitive.type = PRIM_TRIANGLE;
    primitive.shading = PRIM_SHADE_GOURAUD;
    primitive.mapping = false;
    primitive.fogging = false;
    primitive.blending = false;
    primitive.mapping_type = PRIM_MAP_ST;
    primitive.colorfix = PRIM_UNFIXED;

    const u64 regList = (
        (GIF_REG_RGBAQ  & 0xF) << 0      |
        (GIF_REG_XYZ2   & 0xF) << 4      |
        (GIF_REG_RGBAQ  & 0xF) << 8      |
        (GIF_REG_XYZ2   & 0xF) << 12     |
        (GIF_REG_RGBAQ  & 0xF) << 16     |
        (GIF_REG_XYZ2   & 0xF) << 20
    );

    packet2_utils_gs_add_prim_giftag(
        packet,
        &primitive,
        1,              // Loop Count
        regList,
        3 * 2,          // Number of registers per loop
        0               // Context
    );
}
#define PACKED_XYZ2(X, Y, Z, ADC) ((u128(ADC)<<111) | (u128(Z)<<64) | (u128(Y)<<32) | (u128(X)))
#define PACKED_RGBA(R, G, B, A) ((u128(A)<<96) | (u128(B)<<64) | (u128(G)<<32) | (u128(R)))

inline void addVertex(packet2_t* packet, glm::vec4 pos, glm::vec4 color)
{
    packet2_add_s128(packet, PACKED_RGBA(
        255,0, 0, 255
    ));

    const float offsetX = 20.0f;
    const float offsetY = 20.0f;

    packet2_add_s128(packet, PACKED_XYZ2(
        u32(offsetX + pos.x) << 4,
        u32(offsetY + pos.y) << 4,
        128,
        0
    ));
}

void savePacket(packet2_t* packet, const char* filename)
{
    std::ofstream file(filename, std::ofstream::binary);

    file.write((const char*)packet->base, packet2_get_qw_count(packet) * 16);
}

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color
) {    
    printf("        Draw TRI\n");

    packet2_t* packet = packets[context];

    packet2_reset(packet, 0);
    packet2_chain_open

    static triangle_t tri;
    tri.v0.x = 0.0f;
    tri.v0.y = 0.0f;
    tri.v0.z = 0;
    
    tri.v1.x = 2048.0f;
    tri.v1.y = 0.0f;
    tri.v1.z = 0;

    tri.v2.x = 0.0f;
    tri.v2.y = 2048.0f;
    tri.v2.z = 0;

    tri.color.r = 0;
    tri.color.g = 128;
    tri.color.b = 0;

    packet2_update(
        packet,
        draw_triangle_filled(packet->next, 0, &tri)
    );
    packet2_update(packet, draw_finish(packet->next));

    dma_channel_wait(DMA_CHANNEL_GIF, 0);
    dma_channel_send_packet2(packet, DMA_CHANNEL_GIF, 1);

    context ^= 1;
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
    packet2_t* packet = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 1);

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

	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

    initGs();
    initDrawingEnvironment();

    packets[0] = packet2_create(1024, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
    packets[1] = packet2_create(1024, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

    return true;
}

void close()
{

}


} // namespace ps2::draw