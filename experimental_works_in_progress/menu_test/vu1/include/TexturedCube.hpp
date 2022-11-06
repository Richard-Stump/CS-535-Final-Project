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

using VECTOR = float[4];
using MATRIX = float[16];

class TexturedCube
{
private:
    packet2_t* cube_packet;
    texbuffer_t texbuffer;

    prim_t prim;
    clutbuffer_t clut;
    lod_t lod;

    // int faces_count = 2;

    VECTOR position = {0.00f, 0.00f, 0.00f, 1.00f};
    VECTOR rotation = {0.00f, 0.00f, 0.00f, 1.00f};

public:
    TexturedCube();
    ~TexturedCube();

    void init();
    packet2_t* get_packet();
    texbuffer_t get_texbuffer();
    texbuffer_t* get_texbuffer_ptr();
    void deleet();
    void add_position(int, float);
    void add_rotation(int, float);
    void set_position(int, float);
    float* get_position();
    float* get_rotation();
    prim_t get_prim();
    prim_t* get_prim_ptr();
    clutbuffer_t get_clut();
    clutbuffer_t* get_clut_ptr();
    lod_t get_lod();
    lod_t* get_lod_ptr();
    void draw(MATRIX local_world, MATRIX world_view, MATRIX view_screen, MATRIX local_screen, VECTOR camera_position, VECTOR camera_rotation, packet2_t **vif_packets, packet2_t *curr_vif_packet, u8 context, VECTOR* c_verts, VECTOR* c_sts);
    void calculate();
};