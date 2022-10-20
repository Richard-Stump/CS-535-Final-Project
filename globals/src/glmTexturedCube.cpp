#include "../include/glmglmTexturedCube.hpp"

glmTexturedCube::glmTexturedCube()
{
    init();
	position[0] = 130.0f;
	position[1] = 140.0f;
	position[2] = 0.0f;
	position[3] = 1.0;
    cube_packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
}

void glmTexturedCube::init()
{
    packet2_add_float(cube_packet, 2048.0F);					  // scale
	packet2_add_float(cube_packet, 2048.0F);					  // scale
	packet2_add_float(cube_packet, ((float)0xFFFFFF) / 32.0F); // scale
	packet2_add_s32(cube_packet, faces_count);				  // vertex count
	packet2_utils_gif_add_set(cube_packet, 1);
	packet2_utils_gs_add_lod(cube_packet, &lod);
	packet2_utils_gs_add_texbuff_clut(cube_packet, &texbuffer, &clut);
	packet2_utils_gs_add_prim_giftag(cube_packet, &prim, faces_count, DRAW_STQ2_REGLIST, 3, 0);
	u8 j = 0; // RGBA
	for (j = 0; j < 4; j++)
		packet2_add_u32(cube_packet, 128);

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

	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_ENABLE;
	prim.fogging = DRAW_DISABLE;
	prim.blending = DRAW_ENABLE;
	prim.antialiasing = DRAW_DISABLE;
	prim.mapping_type = PRIM_MAP_ST;
	prim.colorfix = PRIM_UNFIXED;

	texbuffer.width = 128;
	texbuffer.psm = GS_PSM_24;
	texbuffer.address = graph_vram_allocate(128, 128, GS_PSM_24, GRAPH_ALIGN_BLOCK);
	texbuffer.info.width = draw_log2(128);
	texbuffer.info.height = draw_log2(128);
	texbuffer.info.components = TEXTURE_COMPONENTS_RGB;
	texbuffer.info.function = TEXTURE_FUNCTION_DECAL;
}

packet2_t* glmTexturedCube::get_packet()
{
	return cube_packet;
}

texbuffer_t glmTexturedCube::get_texbuffer()
{
	return texbuffer;
}

texbuffer_t* glmTexturedCube::get_texbuffer_ptr()
{
	return &texbuffer;
}

void glmTexturedCube::deleet()
{
	packet2_free(cube_packet);
}

void glmTexturedCube::add_position(Vec4 v)
{
	position += v;
}

void glmTexturedCube::add_rotation(Vec4 v)
{
	rotation += v;
}


Vec4 glmTexturedCube::get_position()
{
	return position;
}

void glmTexturedCube::set_position(Vec4 v)
{
	position = v;
}

Vec4 glmTexturedCube::get_rotation()
{
	return rotation;
}

prim_t glmTexturedCube::get_prim()
{
	return prim;
}

prim_t* glmTexturedCube::get_prim_ptr()
{
	return &prim;
}

clutbuffer_t glmTexturedCube::get_clut()
{
	return clut;
}

clutbuffer_t* glmTexturedCube::get_clut_ptr()
{
	return &clut;
}

lod_t glmTexturedCube::get_lod()
{
	return lod;
}

lod_t* glmTexturedCube::get_lod_ptr()
{
	return &lod;
}

void glmTexturedCube::draw(MATRIX local_world, MATRIX world_view, MATRIX view_screen, MATRIX local_screen, VECTOR camera_position, VECTOR camera_rotation, packet2_t **vif_packets, packet2_t *curr_vif_packet, u8 context, VECTOR* c_verts, VECTOR* c_sts)
{
	create_local_world(local_world, position, rotation);
	create_world_view(world_view, camera_position, camera_rotation);
	create_local_screen(local_screen, local_world, world_view, view_screen);
	curr_vif_packet = vif_packets[context];
	packet2_reset(curr_vif_packet, 0);

	// Add matrix at the beggining of VU mem (skip TOP)
	packet2_utils_vu_add_unpack_data(curr_vif_packet, 0, &local_screen, 8, 0);

	u32 vif_added_bytes = 0; // zero because now we will use TOP register (double buffer)
							 // we don't wan't to unpack at 8 + beggining of buffer, but at
							 // the beggining of the buffer

	// Merge packets
	packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, cube_packet->base, packet2_get_qw_count(cube_packet), 1);
	vif_added_bytes += packet2_get_qw_count(cube_packet);

	// Add vertices
	packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, c_verts, faces_count, 1);
	vif_added_bytes += faces_count; // one VECTOR is size of qword

	// Add sts
	packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, c_sts, faces_count, 1);
	vif_added_bytes += faces_count;

	packet2_utils_vu_add_start_program(curr_vif_packet, 0);
	packet2_utils_vu_add_end_tag(curr_vif_packet);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	dma_channel_send_packet2(curr_vif_packet, DMA_CHANNEL_VIF1, 1);

	// Switch packet, so we can proceed during DMA transfer
	context = !context;
}

Mat4 glmTexturedCube::get_model()
{
	return model;
}

void glmTexturedCube::set_model(Mat4 _model)
{
	model = _model;
}