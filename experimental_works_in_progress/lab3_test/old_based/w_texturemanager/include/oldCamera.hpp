#pragma once

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

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

#define GRAVITY 9.8

class oldCamera
{
public:
    framebuffer_t frame;
    zbuffer_t z;

    VECTOR position;
    VECTOR rotation;

    MATRIX world_view, view_screen, local_screen;

    // float up[3] = {0,1,0};

    packet2_t *vif_packets[2] __attribute__((aligned(64)));
    packet2_t *curr_vif_packet;
    u8 context = 0;

    bool falling = false;

    float rotation_sensitivity = 0.0005f;
	float walk_sensitivity = 0.005f;

    float standing_jump_speed = 2.7;
    float running_jump_speed = 9.15;
    float gravity_per_frame = GRAVITY / 50.0f;
    float walking_speed_per_second = 1.42 / 50.0f;
    float current_jump_speed = 0;
    float pre_jump_speed_vector[3] = {0,0,0};
    float pre_jump_rotation_vector[3] = {0,0,0};

    void init_gs();
    void init_drawing_environment();
    void free_packets();

    void clear();
    void boundary(float, float, float, float);
    void boundary(float, float, float, float, float, float);
    void set_pre_jump_speed(float, float);
    // void boundary(float[4]);
    // void boundary(float[6]);
    // void update();

    oldCamera();
    oldCamera(float, float, float, float);
    ~oldCamera();
};