#include "../include/TexturedShape.hpp"

TexturedShape::TexturedShape()
{
    packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	set_textured();
}

TexturedShape::~TexturedShape()
{

}

void TexturedShape::send_texture(unsigned char* texture_array)
{
    packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
	packet2_update(packet2, draw_texture_transfer(packet2->next, texture_array, 2, 2, GS_PSM_24, texbuffer.address, texbuffer.width));
	packet2_update(packet2, draw_texture_flush(packet2->next));
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();
	packet2_free(packet2);
}

void TexturedShape::set_textured()
{

    texbuffer.width = 2;
    texbuffer.psm = GS_PSM_24;
    texbuffer.address = graph_vram_allocate(2, 2, GS_PSM_24, GRAPH_ALIGN_BLOCK);
}

void TexturedShape::set_lod_clut_prim_tex_buff()
{
	lod.calculation = LOD_USE_K;
	lod.max_level = 0;
	lod.mag_filter = LOD_MAG_NEAREST;
	lod.min_filter = LOD_MIN_NEAREST;
	lod.l = 0;
	lod.k = 0;

	clut.storage_mode = CLUT_STORAGE_MODE1;
	clut.start = 0;
	clut.psm = 0;
	clut.load_method = CLUT_NO_LOAD;
	clut.address = 0;

	// Define the triangle primitive we want to use.
	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_ENABLE;
	prim.fogging = DRAW_DISABLE;
	prim.blending = DRAW_ENABLE;
	prim.antialiasing = DRAW_DISABLE;
	prim.mapping_type = PRIM_MAP_ST;
	prim.colorfix = PRIM_UNFIXED;

	texbuffer.info.width = draw_log2(2);
	texbuffer.info.height = draw_log2(2);
	texbuffer.info.components = TEXTURE_COMPONENTS_RGBA;
	texbuffer.info.function = TEXTURE_FUNCTION_MODULATE;
}

void TexturedShape::send_vertices()
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