#pragma once

#include "glm/glm.hpp"
#include "PS2Pad.hpp"
#include "Menu.hpp"

#include <string>

#include "../assets/purdue_logo.cpp"
#include "../assets/mesh_data.cpp"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "./gs/gsKit.h"
#include "./dma/include/dmaKit.h"
#include "./toolkit/include/gsToolkit.h"

#include "/usr/local/ps2dev/ps2sdk/ports/include/jpeglib.h"
#include "/usr/local/ps2dev/ps2sdk/ports/include/png.h"

#include <kernel.h>
#include <stdio.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>
#include <gif_tags.h>

#include <gs_gp.h>
#include <gs_psm.h>

#include <dma.h>
#include <dma_tags.h>
#include <packet2.h>
#include <packet2_utils.h>

#include <draw.h>
#include <graph.h>
#include <packet.h>
// #include <

#ifdef __cplusplus
}
#endif//__cplusplus

using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Color4 = glm::vec4;