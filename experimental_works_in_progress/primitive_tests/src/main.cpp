#include "../include/main.hpp"



VECTOR object_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
VECTOR camera_position = {140.00f, 140.00f, 320.00f, 1.00f};
VECTOR camera_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
MATRIX local_world, world_view, view_screen, local_screen;

/** 
 * Packets for sending VU data 
 * Each packet will have: 
 * a) View/Projection matrix (calculated every frame) 
 * b) Cube data (prim,lod,vertices,sts,...) added from zbyszek_packet. 
 */
packet2_t *vif_packets[2] __attribute__((aligned(64)));
packet2_t *curr_vif_packet;

/** Cube data */
packet2_t *zbyszek_packet;

u8 context = 0;

/** Set GS primitive type of drawing. */
prim_t prim;

/** 
 * Color look up table. 
 * Needed for texture. 
 */
clutbuffer_t clut;

/** 
 * Level of details. 
 * Needed for texture. 
 */
lod_t lod;

/** 
 * Helper arrays. 
 * Needed for calculations. 
 */
VECTOR *c_verts __attribute__((aligned(128))), *c_sts __attribute__((aligned(128)));

void set_lod_clut_prim_tex_buff(texbuffer_t *t_texbuff)
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

	t_texbuff->info.width = draw_log2(128);
	t_texbuff->info.height = draw_log2(128);
	t_texbuff->info.components = TEXTURE_COMPONENTS_RGBA;
	t_texbuff->info.function = TEXTURE_FUNCTION_DECAL;
}

void render(framebuffer_t *t_frame, zbuffer_t *t_z, texbuffer_t *t_texbuff)
{
	int i, j;

	set_lod_clut_prim_tex_buff(t_texbuff);
	c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * 3);
	c_sts = (VECTOR *)memalign(128, sizeof(VECTOR) * 3);
	VECTOR c_zbyszek_position;

	for (i = 0; i < 3; i++)
	{
		c_verts[i][0] = vertices[faces[i]][0];
		c_verts[i][1] = vertices[faces[i]][1];
		c_verts[i][2] = vertices[faces[i]][2];
		c_verts[i][3] = vertices[faces[i]][3];

		c_sts[i][0] = sts[faces[i]][0];
		c_sts[i][1] = sts[faces[i]][1];
		c_sts[i][2] = sts[faces[i]][2];
		c_sts[i][3] = sts[faces[i]][3];
	}

    // calculate_cube(t_texbuff);
    packet2_add_float(zbyszek_packet, 2048.0F);					  // scale
	packet2_add_float(zbyszek_packet, 2048.0F);					  // scale
	packet2_add_float(zbyszek_packet, ((float)0xFFFFFF) / 32.0F); // scale
	packet2_add_s32(zbyszek_packet, faces_count);				  // vertex count
	packet2_utils_gif_add_set(zbyszek_packet, 1);
	packet2_utils_gs_add_lod(zbyszek_packet, &lod);
	packet2_utils_gs_add_texbuff_clut(zbyszek_packet, t_texbuff, &clut);
	packet2_utils_gs_add_prim_giftag(zbyszek_packet, &prim, faces_count, DRAW_STQ2_REGLIST, 3, 0);
	
	u8 j2 = 0; // RGBA
	for (j2 = 0; j2 < 4; j2++)
		packet2_add_u32(zbyszek_packet, 128);

	// The main loop...
	for (;;)
	{
		if (camera_position[2] >= 400.0F)
		{
			camera_position[2] = 40.0F;
			camera_rotation[2] = 0.00f;
		}

		clear_screen(t_frame, t_z);

        c_zbyszek_position[0] = 160.0F;
        c_zbyszek_position[1] = 160.0F;
        
        // draw_cube(c_zbyszek_position, t_texbuff);
        create_local_world(local_world, c_zbyszek_position, object_rotation);
        create_world_view(world_view, camera_position, camera_rotation);
        create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);
        create_local_screen(local_screen, local_world, world_view, view_screen);
        curr_vif_packet = vif_packets[context];
        packet2_reset(curr_vif_packet, 0);

        // Add matrix at the beggining of VU mem (skip TOP)
        packet2_utils_vu_add_unpack_data(curr_vif_packet, 0, &local_screen, 8, 0);

        u32 vif_added_bytes = 0; // zero because now we will use TOP register (double buffer)
                                // we don't wan't to unpack at 8 + beggining of buffer, but at
                                // the beggining of the buffer

        // Merge packets
        packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, zbyszek_packet->base, packet2_get_qw_count(zbyszek_packet), 1);
        vif_added_bytes += packet2_get_qw_count(zbyszek_packet);
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

		graph_wait_vsync();
	}
}

int main(int argc, char *argv[])
{

	// Init DMA channels.
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

	// Initialize vif packets
	zbyszek_packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	vu1_upload_micro_program();
	vu1_set_double_buffer_settings();

	// The buffers to be used.
	framebuffer_t frame;
	zbuffer_t z;
	texbuffer_t texbuff;

	init_gs(&frame, &z, &texbuff);
	init_drawing_environment(&frame, &z);
	send_texture(&texbuff, purdue_logo);

	// Render textured cube
	render(&frame, &z, &texbuff);

	packet2_free(vif_packets[0]);
	packet2_free(vif_packets[1]);
	packet2_free(zbyszek_packet);

	// Sleep
	SleepThread();

	// End program.
	return 0;
}