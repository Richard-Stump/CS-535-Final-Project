#ifndef FP_LIB_H
#define FP_LIB_H

// Any of the SDK headers that are not part of GCC's libraries need to be in an
// extern "C" block to prevent C++ name mangling. 
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

#include <draw.h>
#include <graph.h>
#include <packet.h>

#ifdef __cplusplus
}
#endif//__cplusplus

void debug_text(int x, int y, const char* string, int t);
void project_title();
void init_gs(framebuffer_t *frame, zbuffer_t *z);
void init_drawing_environment(packet_t *packet, framebuffer_t *frame, zbuffer_t *z);
void draw_single_rect(
                        packet_t* packet,
                        qword_t* q,
                        int xmin,
                        int xmax,
                        int ymin, 
                        int ymax, 
                        int r, 
                        int g, 
                        int b,
                        int a, 
                        float t
                    );
void render(packet_t *packet, framebuffer_t *frame);

#endif