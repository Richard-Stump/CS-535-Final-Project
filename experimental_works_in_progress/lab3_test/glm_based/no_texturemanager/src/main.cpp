#include "../include/main.hpp"

VECTOR *c_verts __attribute__((aligned(128))), *c_sts __attribute__((aligned(128)));

extern unsigned char purdue_logo[];

packet2_t *zbyszek_packet;
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

packet2_t *vif_packets[2] __attribute__((aligned(64)));
packet2_t *curr_vif_packet;

u8 context = 0;
VECTOR object_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
VECTOR camera_position = {140.00f, 140.00f, 320.00f, 1.00f};
VECTOR camera_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
MATRIX local_world, world_view, view_screen, local_screen;

void send_texture(texbuffer_t *texbuf)
{
	packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
	packet2_update(packet2, draw_texture_transfer(packet2->next, purdue_logo, 128, 128, GS_PSM_24, texbuf->address, texbuf->width));
	packet2_update(packet2, draw_texture_flush(packet2->next));
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();
	packet2_free(packet2);
}

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
	t_texbuff->info.components = TEXTURE_COMPONENTS_RGB;
	t_texbuff->info.function = TEXTURE_FUNCTION_DECAL;
}

void calculate_cube(texbuffer_t *t_texbuff)
{
	packet2_add_float(zbyszek_packet, 2048.0F);					  // scale
	packet2_add_float(zbyszek_packet, 2048.0F);					  // scale
	packet2_add_float(zbyszek_packet, ((float)0xFFFFFF) / 32.0F); // scale
	packet2_add_s32(zbyszek_packet, faces_count);				  // vertex count
	
	
	packet2_utils_gif_add_set(zbyszek_packet, 1);
	
	
	packet2_utils_gs_add_lod(zbyszek_packet, &lod);
	
	
	packet2_utils_gs_add_texbuff_clut(zbyszek_packet, t_texbuff, &clut);
	
	
	packet2_utils_gs_add_prim_giftag(zbyszek_packet, &prim, faces_count, DRAW_STQ2_REGLIST, 3, 0);
	
	
	
	u8 j = 0; // RGBA
	for (j = 0; j < 4; j++)
		packet2_add_u32(zbyszek_packet, 128);
}

void draw_cube(VECTOR t_object_position, texbuffer_t *t_texbuff)
{
	create_local_world(local_world, t_object_position, object_rotation);
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
}

void render(Camera* cam, texbuffer_t *t_texbuff)
{
	int i, j;

	set_lod_clut_prim_tex_buff(t_texbuff);

	/** 
	 * Allocate some space for object position calculating. 
	 * c_ prefix = calc_
	 */
	c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * faces_count);
	c_sts = (VECTOR *)memalign(128, sizeof(VECTOR) * faces_count);

	VECTOR c_zbyszek_position;

	for (i = 0; i < faces_count; i++)
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

	// Create the view_screen matrix.
	// create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);
	calculate_cube(t_texbuff);

	// The main loop...
	for (;;)
	{
		// // Spin the cube a bit.
		// object_rotation[0] += 0.008f;
		// while (object_rotation[0] > 3.14f)
		// {
		// 	object_rotation[0] -= 6.28f;
		// }
		// object_rotation[1] += 0.012f;
		// while (object_rotation[1] > 3.14f)
		// {
		// 	object_rotation[1] -= 6.28f;
		// }

		// camera_position[2] += .5F;
		// camera_rotation[2] += 0.002f;
		if (cam->position[2] >= 400.0F)
		{
			cam->position[2] = 40.0F;
			cam->rotation[2] = 0.00f;
		}

		// clear_screen(cam->frame, cam->z);
        cam->clear();

		// for (i = 0; i < 8; i++)
		// {
			c_zbyszek_position[0] = 160.0F;
			// for (j = 0; j < 8; j++)
			// {
				c_zbyszek_position[1] = 160.0F;
				draw_cube(c_zbyszek_position, t_texbuff);
			// }
		// }

		graph_wait_vsync();
	}
}

void render(Camera* cam)
{
    cam->clear();
    // cam->ins(gamepad);
    cam->M(60.0f, 0.01f, 150.0f);
    cam->boundary();

    cam->step();
	graph_wait_vsync();
}

void init_gs(texbuffer_t *t_texbuff)
{
	// Allocate some vram for the texture buffer
	t_texbuff->width = 128;
	t_texbuff->psm = GS_PSM_24;
	t_texbuff->address = graph_vram_allocate(128, 128, GS_PSM_24, GRAPH_ALIGN_BLOCK);
}

int main()
{
	SifInitRpc(0);
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

    packet2_t *vif_packets[2] __attribute__((aligned(64)));
    packet2_t *curr_vif_packet;
    u8 context = 0;


	zbyszek_packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

    vu1_upload_micro_program();
	vu1_set_double_buffer_settings();

    // framebuffer_t frame;
	// zbuffer_t z;
    texbuffer_t texbuffer;

    float bounds[6] = { -10,10,
                        -1, 1,
                        -10,10
                        };

    Camera cam(480, 640, Vec3(140,140,40));
    cam.set_bounds(bounds);

    PS2Pad gamepad;
    gamepad.init(0,0);

    TexturedCube cube;

    cam.init_gs();
    // cube.init_gs()
    init_gs(&texbuffer);

    cam.init_drawing_environment();

    send_texture(&texbuffer);
    // cube.send_texture(purdue_logo);

    while(1)
    {
        render(&cam, &texbuffer);
        gamepad.read();
        if(gamepad.clicked(PAD_UP)) printf("up!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
        if(gamepad.clicked(PAD_DOWN)) printf("down!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");
    }



	packet2_free(vif_packets[0]);
	packet2_free(vif_packets[1]);

    SleepThread();

    return 0;
}