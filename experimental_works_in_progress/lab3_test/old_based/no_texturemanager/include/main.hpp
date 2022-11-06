#pragma once

// #include "../../../globals/include/glm/glm.hpp"
#include "../../../../../globals/include/PS2Pad.hpp"
#include "../../../../../globals/include/OBJ_Loader.hpp"
#include "TexturedCubeMesh.hpp"
#include "oldCamera.hpp"
// #include "glm_modified_functions.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <cmath>
// #include "../../../../../globals/assets/purdue_logo.c"
// #include "../../../../../globals/assets/bedrich.c"
// #include "font.h"
// #include "texture.h"
// #include "../assets/bedrich.c"


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
// #include "ref_ps2.h"
// #include "defs_ps2.h"

// #include "/usr/local/ps2dev/ps2sdk/ports/include/jpeglib.h"
// #include "/usr/local/ps2dev/ps2sdk/ports/include/png.h"
// #include "../../../globals/assets/cube_mesh_data.c"




#include <sifcmd.h>
#include <rom0_info.h>
#include <stdarg.h>
#include <kernel.h>
#include <stdio.h>
#include <stdarg.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>
#include <gif_tags.h>

#include <gs_gp.h>
#include <gs_psm.h>

// #include "../include/gsKit/gs/gsKit.h"
// #include "../include/gsKit/dma/dmaKit.h"
// #include "../include/gsKit/toolkit/gsToolkit.h"

#include <dma.h>
#include <dma_tags.h>
#include <packet2.h>
#include <packet2_utils.h>

#include <draw.h>
#include <graph.h>
#include <packet.h>
#define GRAPH_MODE_NTSC         1
// #include <

#ifdef __cplusplus
}
#endif//__cplusplus

// using Vec3 = glm::vec3;
// using Vec4 = glm::vec4;
// using Color4 = glm::vec4;

void vu1_upload_micro_program();
void vu1_set_double_buffer_settings();

// void render(oldCamera*, TexturedCube*);
// void draw_cube(oldCamera*, TexturedCube*);
