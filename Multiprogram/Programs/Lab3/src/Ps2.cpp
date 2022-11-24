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

#include <gsKit.h>

namespace ps2::draw {

GSGLOBAL* gsGlobal;

void flipScreen()
{
    gsKit_queue_exec(gsGlobal);
    gsKit_sync_flip(gsGlobal);
}

void initGraphics()
{
    gsGlobal = gsKit_init_global();

    gsGlobal->ZBuffering = GS_SETTING_ON;
	//gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	//gsGlobal->PrimAAEnable = GS_SETTING_ON;

	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);

	gsKit_vram_clear(gsGlobal);

	gsKit_init_screen(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
}

void beginFrame()
{
    gsKit_clear(
        gsGlobal, 
        GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00)
    );
}

void endFrame()
{
    flipScreen();
}

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color
) {    
    for(int i = 0; i < verts.size(); i += 3) {
        glm::vec4 v1 = verts[i];
        glm::vec4 v2 = verts[i + 1];
        glm::vec4 v3 = verts[i + 2];

        // Transform
        v1 = matTrans * v1;
        v2 = matTrans * v2;
        v3 = matTrans * v3;

        if (v1.z <= 0.0f ||
            v2.z <= 0.0f ||
            v3.z <= 0.0f
        ) {
            continue;
        }

        // perspective divide
        v1 = v1 / v1.w;
        v2 = v2 / v2.w;
        v3 = v3 / v3.w;
        
        if(
            (v1.x < -1.0f && v2.x < -1.0f && v3.x < -1.0f) ||
            (v1.x > 1.0f && v2.x > 1.0f && v3.x > 1.0f)
        ) {
            continue;
        }

        float cDepth1 = glm::clamp((1.0f - v1.z) * 50.0f, 0.05f, 1.0f);
        float cDepth2 = glm::clamp((1.0f - v2.z) * 50.0f, 0.05f, 1.0f);
        float cDepth3 = glm::clamp((1.0f - v3.z) * 50.0f, 0.05f, 1.0f);

        // Viewport transform
        v1.x = (1.0f + v1.x) * 320;
        v1.y = (1.0f - v1.y) * 240;
        v2.x = (1.0f + v2.x) * 320;
        v2.y = (1.0f - v2.y) * 240;
        v3.x = (1.0f + v3.x) * 320;
        v3.y = (1.0f - v3.y) * 240;

        glm::vec4 color1 = color * cDepth1;
        glm::vec4 color2 = color * cDepth2;
        glm::vec4 color3 = color * cDepth3;

        const unsigned int max_z = 1 << (32 - 1);
        int z1 = (int)((v1.z + 1.0f) * max_z);
        int z2 = (int)((v2.z + 1.0f) * max_z);
        int z3 = (int)((v3.z + 1.0f) * max_z);

        u64 uColor1 = GS_SETREG_RGBAQ(
            color1.r * 255.0f, 
            color1.g * 255.0f, 
            color1.b * 255.0f, 
            0xFF,
            0x00);
        u64 uColor2 = GS_SETREG_RGBAQ(
            color2.r * 255.0f, 
            color2.g * 255.0f, 
            color2.b * 255.0f, 
            0xFF,
            0x00);
        u64 uColor3 = GS_SETREG_RGBAQ(
            color3.r * 255.0f, 
            color3.g * 255.0f, 
            color3.b * 255.0f, 
            0xFF,
            0x00);

        gsKit_prim_line_goraud_3d(
            gsGlobal,
            v1.x, v1.y,     z1,
            v2.x, v2.y,     z2,
            uColor1,
            uColor2
        );
        
        gsKit_prim_line_goraud_3d(
            gsGlobal,
            v2.x, v2.y,     z2,
            v3.x, v3.y,     z3,
            uColor2,
            uColor3
        );
        
        gsKit_prim_line_goraud_3d(
            gsGlobal,
            v3.x, v3.y,     z3,
            v1.x, v1.y,     z1,
            uColor3,
            uColor1
        );
    }    
}

//==============================================================================
// Initialization code
//==============================================================================


bool init()
{
    initGraphics();

    gsKit_set_test(gsGlobal, GS_ZTEST_ON);
    
    return true;
}

void close()
{

}


} // namespace ps2::draw