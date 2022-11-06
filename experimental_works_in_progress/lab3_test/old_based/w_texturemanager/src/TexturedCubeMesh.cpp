#include "../include/TexturedCubeMesh.hpp"

TexturedCubeMesh::TexturedCubeMesh()
{
    packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

    rotation[0] = 0;
    rotation[1] = 0;
    rotation[2] = 0;
    rotation[3] = 1;
}

TexturedCubeMesh::TexturedCubeMesh(float a, float b, float c, float d)
{
	position[0] = a;
	position[1] = b;
	position[2] = c;
	position[3] = d;
}

void TexturedCubeMesh::calculate()
{
    packet2_add_float(packet, 2048.0F);					  // scale
	packet2_add_float(packet, 2048.0F);					  // scale
	packet2_add_float(packet, ((float)0xFFFFFF) / 32.0F); // scale
	packet2_add_s32(packet, this->face_count);				  // vertex count
	packet2_utils_gif_add_set(packet, 1);
	packet2_utils_gs_add_lod(packet, &lod);
	packet2_utils_gs_add_texbuff_clut(packet, &texbuffer, &clut);
	packet2_utils_gs_add_prim_giftag(packet, &prim, this->face_count, DRAW_STQ2_REGLIST, 3, 0);
	
	u8 j = 0; // RGBA
	for (j = 0; j < 4; j++)
		packet2_add_u32(packet, 128);
}
