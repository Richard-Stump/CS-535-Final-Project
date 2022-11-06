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
#include <packet.h>
#include <malloc.h>
// #include "../../../globals/assets/cube_mesh_data.c"
// VECTOR* c_verts __attribute__((aligned(128))); VECTOR* c_sts __attribute__((aligned(128)));
#include "TexturedShape.hpp"
#include "Mesh.hpp"

class TexturedCubeMesh: public TexturedShape
{
public:
    TexturedCubeMesh();
    TexturedCubeMesh(float, float, float, float);
    ~TexturedCubeMesh();

    int face_count = 36, vert_count = 24;

    int faces[36] = {
    0, 1, 2,
    1, 2, 3,
    4, 5, 6,
    5, 6, 7,
    8, 9, 10,
    9, 10, 11,
    12, 13, 14,
    13, 14, 15,
    16, 17, 18,
    17, 18, 19,
    20, 21, 22,
    21, 22, 23};

    VECTOR vertices[24] = {
    {10.00f, 10.00f, 10.00f, 1.00f},
    {10.00f, 10.00f, -10.00f, 1.00f},
    {10.00f, -10.00f, 10.00f, 1.00f},
    {10.00f, -10.00f, -10.00f, 1.00f},
    {-10.00f, 10.00f, 10.00f, 1.00f},
    {-10.00f, 10.00f, -10.00f, 1.00f},
    {-10.00f, -10.00f, 10.00f, 1.00f},
    {-10.00f, -10.00f, -10.00f, 1.00f},
    {-10.00f, 10.00f, 10.00f, 1.00f},
    {10.00f, 10.00f, 10.00f, 1.00f},
    {-10.00f, 10.00f, -10.00f, 1.00f},
    {10.00f, 10.00f, -10.00f, 1.00f},
    {-10.00f, -10.00f, 10.00f, 1.00f},
    {10.00f, -10.00f, 10.00f, 1.00f},
    {-10.00f, -10.00f, -10.00f, 1.00f},
    {10.00f, -10.00f, -10.00f, 1.00f},
    {-10.00f, 10.00f, 10.00f, 1.00f},
    {10.00f, 10.00f, 10.00f, 1.00f},
    {-10.00f, -10.00f, 10.00f, 1.00f},
    {10.00f, -10.00f, 10.00f, 1.00f},
    {-10.00f, 10.00f, -10.00f, 1.00f},
    {10.00f, 10.00f, -10.00f, 1.00f},
    {-10.00f, -10.00f, -10.00f, 1.00f},
    {10.00f, -10.00f, -10.00f, 1.00f}};

    VECTOR sts[24] = {
    {0.00f, 1.00f, 1.00f, 0.00f},
    {1.00f, 1.00f, 1.00f, 0.00f},
    {0.00f, 0.00f, 1.00f, 0.00f},
    {1.00f, 0.00f, 1.00f, 0.00f},
    {0.00f, 1.00f, 1.00f, 0.00f},
    {1.00f, 1.00f, 1.00f, 0.00f},
    {0.00f, 0.00f, 1.00f, 0.00f},
    {1.00f, 0.00f, 1.00f, 0.00f},
    {0.00f, 1.00f, 1.00f, 0.00f},
    {1.00f, 1.00f, 1.00f, 0.00f},
    {0.00f, 0.00f, 1.00f, 0.00f},
    {1.00f, 0.00f, 1.00f, 0.00f},
    {0.00f, 1.00f, 1.00f, 0.00f},
    {1.00f, 1.00f, 1.00f, 0.00f},
    {0.00f, 0.00f, 1.00f, 0.00f},
    {1.00f, 0.00f, 1.00f, 0.00f},
    {0.00f, 1.00f, 1.00f, 0.00f},
    {1.00f, 1.00f, 1.00f, 0.00f},
    {0.00f, 0.00f, 1.00f, 0.00f},
    {1.00f, 0.00f, 1.00f, 0.00f},
    {0.00f, 1.00f, 1.00f, 0.00f},
    {1.00f, 1.00f, 1.00f, 0.00f},
    {0.00f, 0.00f, 1.00f, 0.00f},
    {1.00f, 0.00f, 1.00f, 0.00f}};

    void calculate();
    void build();
};

