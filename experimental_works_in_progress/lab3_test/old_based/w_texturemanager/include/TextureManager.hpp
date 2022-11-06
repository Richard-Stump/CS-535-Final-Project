#pragma once

// #include <cstdlib>
// #include <cstddef>

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
#include <malloc.h>

#include <gs_gp.h>
#include <gs_psm.h>

#include <dma.h>
#include <dma_tags.h>
#include <packet2.h>
#include <packet2_utils.h>

#include <draw.h>
#include <graph.h>
#include <packet.h>

class TextureManager
{
public:
    unsigned int count;
    texbuffer_t** texbuffer;
    unsigned char** texture_array;
    lod_t* lod;
    clutbuffer_t* clut;
    prim_t* prim;

    TextureManager(unsigned char*);
    // TextureManager(unsigned int);
    ~TextureManager();

    void add();
    void init();
    void send_texture();
};