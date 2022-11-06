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

using COLOR = float[4];

class Shape
{
public:
    int face_count, vert_count;

    VECTOR  position = {0,0,0,1},
            rotation = {0,0,0,1};

    MATRIX  model;

    COLOR   color = {64,64,64,128};

    prim_t prim;
    clutbuffer_t clut;
    lod_t lod;

    packet2_t* packet;

    void free_packet();

};
    