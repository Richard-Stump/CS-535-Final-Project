#pragma once
#include "ps2sdk_incs.hpp"

class glmCamera
{
public:
    unsigned int    width = 640,
                    height = 512;

    Vec3    position = Vec3(0,0,0),
            front = Vec3(0,0,-1),
            up = Vec3(0,1,0);

    Mat4    pv;

    float   pal_speed = 1/50.0,
            ntsc_speed = 1/60.0;

    Color   bg_color = {0.25, 0.25, 0.30, 0.50};

    float   near = 0.1f,
            far = 100.0f,
            fov = 67;

public:
    framebuffer_t frame;
    zbuffer_t z;

    packet2_t *vif_packets[2] __attribute__((aligned(64)));
    packet2_t *curr_vif_packet;
    u8 context = 0;

    void init_gs();
    void init_drawing_environment();
    void free_packets();

    void clear();
    void boundary(float, float, float, float, float, float);
    void set_pre_jump_speed(float, float);

    
public:
    glmCamera();
    glmCamera(Vec3);
    ~glmCamera();

    Mat4 M();

};