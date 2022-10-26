#pragma once

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
#pragma once
#include <packet.h>

using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;

class glmTexturedCube
{
private:
    packet2_t* cube_packet;
    texbuffer_t texbuffer;

    prim_t prim;
    clutbuffer_t clut;
    lod_t lod;

    int faces_count = 2;

    Vec4 position = Vec4(0.00f, 0.00f, 0.00f, 1.00f);
    // Vec4 rotation = Vec4(0.00f, 0.00f, 0.00f, 1.00f);
    Vec3 rotation_direction = Vec3(0,0,0);
    float rotation_magnitude = 0.0f;
    Mat4 model = Mat4(1.0f);

public:
    glmTexturedCube();
    ~glmTexturedCube();

    void init();
    packet2_t* get_packet();
    texbuffer_t get_texbuffer();
    texbuffer_t* get_texbuffer_ptr();
    void deleet();
    void add_position(Vec4);
    void add_rotation(Vec3);
    void set_position(Vec4);
    Vec4 get_position();
    Vec3 get_rotation();
    prim_t get_prim();
    prim_t* get_prim_ptr();
    clutbuffer_t get_clut();
    clutbuffer_t* get_clut_ptr();
    lod_t get_lod();
    lod_t* get_lod_ptr();
    void draw(MATRIX local_world, MATRIX world_view, MATRIX view_screen, MATRIX local_screen, VECTOR camera_position, VECTOR camera_rotation, packet2_t **vif_packets, packet2_t *curr_vif_packet, u8 context, VECTOR* c_verts, VECTOR* c_sts);
    Mat4 get_model();
    void set_model(Mat4);
};