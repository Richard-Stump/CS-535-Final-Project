#pragma once

#include "ps2sdk_incs.hpp"

extern u32 VU1Draw3D_CodeStart __attribute__((section(".vudata")));
extern u32 VU1Draw3D_CodeEnd __attribute__((section(".vudata")));

void vu1_set_double_buffer_settings();
void vu1_upload_micro_program();

void init_gs(framebuffer_t *t_frame, zbuffer_t *t_z, texbuffer_t *t_texbuff);
void init_drawing_environment(framebuffer_t *t_frame, zbuffer_t *t_z);
void send_texture(texbuffer_t *texbuf, unsigned char*);
void clear_screen(framebuffer_t *frame, zbuffer_t *z);
// void set_lod_clut_prim_tex_buff(texbuffer_t *t_texbuff);
