#pragma once

#include <malloc.h>
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

#include "Mesh.hpp"
#include "Shape.hpp"

using COLOR = float[4];

class TexturedShape: public Shape, public Mesh
{
public:
    texbuffer_t texbuffer;
    unsigned char* texture;

    TexturedShape();
    ~TexturedShape();
    void set_lod_clut_prim_tex_buff();
    void set_textured();
    void send_texture(unsigned char* texture_array);
    void send_vertices(); // not working for some reason... looking into it
    void calculate();
};