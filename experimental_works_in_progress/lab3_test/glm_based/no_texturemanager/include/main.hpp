#ifndef MAIN_HPP
#define MAIN_HPP

// #include <iostream>
#include "stdio.h"
#include "../../../globals/include/glm/glm.hpp"
#include "../../../globals/include/glm/gtc/matrix_transform.hpp"
#include "../../../globals/include/glm/gtc/type_ptr.hpp"
// #include <vector>

#include "Camera.hpp"
#include "../../../globals/include/PS2Pad.hpp"
#include "../../../globals/include/TexturedCube.hpp"
using Color3 = glm::vec3;
using Color4 = glm::vec4;
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Mat3 = glm::mat3;

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


extern u32 VU1Draw3D_CodeStart __attribute__((section(".vudata")));
extern u32 VU1Draw3D_CodeEnd __attribute__((section(".vudata")));

#include "../assets/purdue_logo.c"
// #include "../assets/bedrich.c"
#include "../assets/cube_mesh_data.c"


// const int width = 1920, height = 1080;
// const int floor_w = 50, floor_h = 50;
// const int ceiling_w = 25, ceiling_h = 25;
// const float fov = 60.0f;
// const float near = 0.01f, far = 150.0f;

// const float mm_w = 400;

// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

// #ifndef M_PI_2
// #define M_PI_2 1.57079632679489661923
// #endif

// #ifndef GRAVITY
// #define GRAVITY 9.7705078125
// #endif

void vu1_set_double_buffer_settings()
{
	packet2_t *packet2 = packet2_create(1, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_utils_vu_add_double_buffer(packet2, 8, 496);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

void vu1_upload_micro_program()
{
	u32 packet_size =
		packet2_utils_get_packet_size_for_program(&VU1Draw3D_CodeStart, &VU1Draw3D_CodeEnd) + 1; // + 1 for end tag
	packet2_t *packet2 = packet2_create(packet_size, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_vif_add_micro_program(packet2, 0, &VU1Draw3D_CodeStart, &VU1Draw3D_CodeEnd);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}



#endif