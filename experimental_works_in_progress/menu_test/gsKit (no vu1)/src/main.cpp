#include "../include/main.hpp"
extern unsigned char purdue_logo[];

extern u32 VU1Draw3D_CodeStart __attribute__((section(".vudata")));
extern u32 VU1Draw3D_CodeEnd __attribute__((section(".vudata")));

VECTOR object_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
VECTOR camera_position = {140.00f, 140.00f, 40.00f, 1.00f};
VECTOR camera_rotation = {0.00f, 0.00f, 0.00f, 1.00f};
// Vec4 object_rotation = Vec4(0,0,0,1);
// Vec4 camera_position = Vec4(0,0,0,1);
// Vec4 camera_rotation = Vec4(0,0,0,1);
MATRIX local_world, world_view, view_screen, local_screen;

packet2_t *vif_packets[2] __attribute__((aligned(64)));
packet2_t *curr_vif_packet;

packet2_t *purdue_logo_packet;

u8 context = 0;

prim_t prim;

clutbuffer_t clut;
lod_t lod;
VECTOR *c_verts __attribute__((aligned(128))), *c_sts __attribute__((aligned(128)));

void calculate_cube(texbuffer_t *t_texbuff)
{
	packet2_add_float(purdue_logo_packet, 2048.0F);					  // scale
	packet2_add_float(purdue_logo_packet, 2048.0F);					  // scale
	packet2_add_float(purdue_logo_packet, ((float)0xFFFFFF) / 32.0F); // scale
	packet2_add_s32(purdue_logo_packet, faces_count);				  // vertex count
	packet2_utils_gif_add_set(purdue_logo_packet, 1);
	packet2_utils_gs_add_lod(purdue_logo_packet, &lod);
	packet2_utils_gs_add_texbuff_clut(purdue_logo_packet, t_texbuff, &clut);
	packet2_utils_gs_add_prim_giftag(purdue_logo_packet, &prim, faces_count, DRAW_STQ2_REGLIST, 3, 0);
	u8 j = 0; // RGBA
	for (j = 0; j < 4; j++)
		packet2_add_u32(purdue_logo_packet, 128);
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
	packet2_utils_vu_add_unpack_data(curr_vif_packet, vif_added_bytes, purdue_logo_packet->base, packet2_get_qw_count(purdue_logo_packet), 1);
	vif_added_bytes += packet2_get_qw_count(purdue_logo_packet);

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

void init_gs(framebuffer_t *t_frame, zbuffer_t *t_z, texbuffer_t *t_texbuff)
{
	// Define a 32-bit 640x512 framebuffer.
	t_frame->width = 640;
	t_frame->height = 512;
	t_frame->mask = 0;
	t_frame->psm = GS_PSM_32;
	t_frame->address = graph_vram_allocate(t_frame->width, t_frame->height, t_frame->psm, GRAPH_ALIGN_PAGE);

	// Enable the zbuffer.
	t_z->enable = DRAW_ENABLE;
	t_z->mask = 0;
	t_z->method = ZTEST_METHOD_GREATER_EQUAL;
	t_z->zsm = GS_ZBUF_32;
	t_z->address = graph_vram_allocate(t_frame->width, t_frame->height, t_z->zsm, GRAPH_ALIGN_PAGE);

	// Allocate some vram for the texture buffer
	t_texbuff->width = 128;
	t_texbuff->psm = GS_PSM_24;
	t_texbuff->address = graph_vram_allocate(128, 128, GS_PSM_24, GRAPH_ALIGN_BLOCK);

	// Initialize the screen and tie the first framebuffer to the read circuits.
	graph_initialize(t_frame->address, t_frame->width, t_frame->height, t_frame->psm, 0, 0);
}

void init_drawing_environment(framebuffer_t *t_frame, zbuffer_t *t_z)
{
	packet2_t *packet2 = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// This will setup a default drawing environment.
	packet2_update(packet2, draw_setup_environment(packet2->next, 0, t_frame, t_z));

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	packet2_update(packet2, draw_primitive_xyoffset(packet2->next, 0, (2048 - 320), (2048 - 256)));

	// Finish setting up the environment.
	packet2_update(packet2, draw_finish(packet2->next));

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();

	packet2_free(packet2);
}

void send_texture(texbuffer_t *texbuf)
{
	packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
	packet2_update(packet2, draw_texture_transfer(packet2->next, purdue_logo, 128, 128, GS_PSM_24, texbuf->address, texbuf->width));
	packet2_update(packet2, draw_texture_flush(packet2->next));
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();
	packet2_free(packet2);
}

void clear_screen(framebuffer_t *frame, zbuffer_t *z)
{
	packet2_t *clear = packet2_create(35, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// Clear framebuffer but don't update zbuffer.
	packet2_update(clear, draw_disable_tests(clear->next, 0, z));
	packet2_update(clear, draw_clear(clear->next, 0, 2048.0f - 320.0f, 2048.0f - 256.0f, frame->width, frame->height, 0x40, 0x40, 0x40));
	packet2_update(clear, draw_enable_tests(clear->next, 0, z));
	packet2_update(clear, draw_finish(clear->next));

	// Now send our current dma chain.
	dma_wait_fast();
	dma_channel_send_packet2(clear, DMA_CHANNEL_GIF, 1);

	packet2_free(clear);

	// Wait for scene to finish drawing
	draw_wait_finish();
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

void vu1_set_double_buffer_settings()
{
	packet2_t *packet2 = packet2_create(1, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_utils_vu_add_double_buffer(packet2, 8, 496);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

void vu1_upload_micro_program()
{
	u32 packet_size =
		packet2_utils_get_packet_size_for_program(&VU1Draw3D_CodeStart, &VU1Draw3D_CodeEnd) + 1; // + 1 for end tag
	packet2_t *packet2 = packet2_create(packet_size, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	packet2_vif_add_micro_program(packet2, 0, &VU1Draw3D_CodeStart, &VU1Draw3D_CodeEnd);
	packet2_utils_vu_add_end_tag(packet2);
	dma_channel_send_packet2(packet2, DMA_CHANNEL_VIF1, 1);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	packet2_free(packet2);
}

void render(framebuffer_t *t_frame, zbuffer_t *t_z, texbuffer_t *t_texbuff, GSGLOBAL* context)
{
	// gsKit_clear(context, GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00));
		
		//////
		//////

		// gsKit_queue_exec(context);
		// gsKit_sync_flip(context);
		// gsKit_TexManager_nextFrame(context);
	printf("rendering cube");
	int i, j;

	set_lod_clut_prim_tex_buff(t_texbuff);

	/** 
	 * Allocate some space for object position calculating. 
	 * c_ prefix = calc_
	 */
	c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * faces_count);
	c_sts = (VECTOR *)memalign(128, sizeof(VECTOR) * faces_count);

	VECTOR c_purdue_logo_position;

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
	create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);
	calculate_cube(t_texbuff);

	// The main loop...
	for (;;)
	{
		// Spin the cube a bit.
		object_rotation[0] += 0.008f;
		while (object_rotation[0] > 3.14f)
		{
			object_rotation[0] -= 6.28f;
		}
		object_rotation[1] += 0.012f;
		while (object_rotation[1] > 3.14f)
		{
			object_rotation[1] -= 6.28f;
		}

		camera_position[2] += .5F;
		camera_rotation[2] += 0.002f;
		if (camera_position[2] >= 400.0F)
		{
			camera_position[2] = 40.0F;
			camera_rotation[2] = 0.00f;
		}

		clear_screen(t_frame, t_z);

		for (i = 0; i < 8; i++)
		{
			c_purdue_logo_position[0] = i * 40.0F;
			for (j = 0; j < 8; j++)
			{
				c_purdue_logo_position[1] = j * 40.0F;
				draw_cube(c_purdue_logo_position, t_texbuff);
			}
		}

		graph_wait_vsync();
	}
}


int main()
{
	SifInitRpc(0);
	PS2Pad paddie;
	paddie.init(0,0);

	u64 Grey, White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSGLOBAL *gsGlobal = gsKit_init_global();
	
	GSTEXTURE test;
	GSFONTM *gsFontM = gsKit_init_fontm();

    dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);


	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);

	WhiteFont = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
	BlackFont = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	RedFont = GS_SETREG_RGBAQ(0xFF,0x80,0x80,0x80,0x00);
	GreenFont = GS_SETREG_RGBAQ(0x80,0xFF,0x80,0x80,0x00);
	BlueFont = GS_SETREG_RGBAQ(0x80,0x80,0xFF,0x80,0x00);
	u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

        float x = 10;
        float y = 10;
        float width = 150;
        float height = 150;

	char tempstr[256];

	float VHeight = gsGlobal->Height;

        float x2 = (gsGlobal->Width - 10) - width;
        float y2 = VHeight - 10 - height;

        gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	
	// gsKit_init_screen(gsGlobal);

	gsKit_fontm_upload(gsGlobal, gsFontM);

	gsFontM->Spacing = 0.95f;

	const char* path = "./test.bmp";
	test.Delayed = 1;
	gsKit_texture_bmp(gsGlobal, &test, path);
	test.Filter = GS_FILTER_LINEAR;

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	purdue_logo_packet = packet2_create(10, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	vu1_upload_micro_program();
	vu1_set_double_buffer_settings();

	framebuffer_t frame;
	zbuffer_t z;
	texbuffer_t texbuff;

	// Init the GS, framebuffer, zbuffer, and texture buffer.
	init_gs(&frame, &z, &texbuff);

	// Init the drawing environment and framebuffer.
	init_drawing_environment(&frame, &z);

	// Load the texture into vram.
	send_texture(&texbuff);


	
	// Initialize the DMAC
	// dmaKit_chan_init(DMA_CHANNEL_GIF);

	// Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	// White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);
	// Grey = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

    // WhiteFont = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);
	// BlackFont = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	// RedFont = GS_SETREG_RGBAQ(0xFF,0x80,0x80,0x80,0x00);
	// GreenFont = GS_SETREG_RGBAQ(0x80,0xFF,0x80,0x80,0x00);
	// BlueFont = GS_SETREG_RGBAQ(0x80,0x80,0xFF,0x80,0x00);
	// u64 TexCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

	// BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	// RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	// GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	// WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

    //     float x = 10;
    //     float y = 10;
    //     float width = 150;
    //     float height = 150;

	// char tempstr[256];

	// float VHeight = gsGlobal->Height;

    //     float x2 = (gsGlobal->Width - 10) - width;
    //     float y2 = VHeight - 10 - height;

    //     gsGlobal->PrimAlphaEnable = GS_SETTING_ON;

	// gsKit_init_screen(gsGlobal);

	// Menu manny(gsGlobal, Vec2(100,100), Vec2(300,500), Color4(0xff,0,0,0xff));
	// manny.set_font(Color4(0,0xff,0,0xff));
	// manny.set_title("Menu Test");
	// // gsKit_fontm_upload(gsGlobal, gsFontM);

	// // gsFontM->Spacing = 0.95f;

	// test.Delayed = 1;
    // const char *path = "./test.bmp";
	// gsKit_texture_bmp(gsGlobal, &test, path);
	// gsKit_texture_jpeg(gsGlobal, &test, path);
	// test.Filter = GS_FILTER_LINEAR;

	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

	 if( y <= 10  && (x + width) < (gsGlobal->Width - 10))
                        x+=5;
                else if( (y + height)  <  (VHeight - 10) && (x + width) >= (gsGlobal->Width - 10) )
                        y+=5;
                else if( (y + height) >=  (VHeight - 10) && x > 10 )
                        x-=5;
                else if( y > 10 && x <= 10 )
                        y-=5;

                if( y2 <= 10  && (x2 + width) < (gsGlobal->Width - 10))
                        x2+=5;
                else if( (y2 + height)  <  (VHeight - 10) && (x2 + width) >= (gsGlobal->Width - 10) )
                        y2+=5;
                else if( (y2 + height) >=  (VHeight - 10) && x2 > 10 )
                        x2-=5;
                else if( y2 > 10 && x2 <= 10 )
                        y2-=5;

	// gsKit_clear(gsGlobal, White);

		// gsKit_TexManager_bind(gsGlobal, &test);
		gsKit_prim_sprite_texture(gsGlobal, &test,
                                        50.0f, 50.0f, 0.0f, 0.0f,
                                        gsGlobal->Width - 50.0f, gsGlobal->Height - 50.0f,
                                        test.Width, test.Height,
                                        1, TexCol);

		gsKit_prim_sprite(gsGlobal, x2, y2, x2 + width, y2 + height, 2, RedTrans);
		gsKit_prim_sprite(gsGlobal, x2, y, x2 + width, y + height, 1, WhiteTrans);


		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 50, 50, 3, 0.85f, WhiteFont,
			"1: ABCDEFGHIJKLM\n"
			"2: NOPQRSTUVWXYZ\n"
			"3: abcdefghijklm\n"
			"4: nopqrstuvwxyz\n"
			"5: 1234567890,./`\n"
			"6: ~!@#$%^&*()_<>\n"
			"7: +-=[]{}\\|;:\"'?\n"
			"8: \ele \ege \einf \emale \efemale \edegc \eyen \ecent \epound\n"
			"9: \eleft \eright \eup \edown \efleft \efright \efup \efdown\n"
			"10:\ehleft \ehright \ehup \ehdown \ems \eus \ens \edegf\n"
			"11:\embit \ehz \ekb \emb \egb \etb \f0855 \f0850");
	// while(1)
	// {
	// 	const char* tmp;

	// 	paddie.read();

	// 	//if (paddie.padRet != 0) {

	// 	if (paddie.clicked(PAD_DOWN)) {
	// 		tmp = "DOWN\n";
	// 	}
	// 	if (paddie.clicked(PAD_RIGHT)) {
	// 		tmp = ("RIGHT\n");
	// 		/*
	// 		   padSetMainMode(port, slot,
	// 		   PAD_MMODE_DIGITAL, PAD_MMODE_LOCK));
	// 		 */
	// 	}
	// 	if (paddie.clicked(PAD_UP)) {
	// 		tmp = ("UP\n");
	// 	}
	// 	if (paddie.clicked(PAD_START)) {
	// 		tmp = ("START\n");
	// 	}
	// 	if (paddie.clicked(PAD_R3)) {
	// 		tmp = ("R3\n");
	// 	}
	// 	if (paddie.clicked(PAD_L3)) {
	// 		tmp = ("L3\n");
	// 	}
	// 	if (paddie.clicked(PAD_SELECT)) {
	// 		tmp = ("SELECT\n");
	// 	}
	// 	if (paddie.clicked(PAD_SQUARE)) {
	// 		tmp = ("SQUARE\n");
	// 		printf("Starting big engine!\n");
	// 		paddie.startBigEngine(255);
	// 	}
	// 	if (paddie.clicked(PAD_CROSS)) {
	// 		paddie.enterPressureMode();
	// 		tmp = ("CROSS - Enter press mode\n");
	// 	}
	// 	if (paddie.clicked(PAD_CIRCLE)) {
	// 		paddie.exitPressureMode();
	// 		tmp = ("CIRCLE - Exit press mode\n");
	// 	}
	// 	if (paddie.clicked(PAD_TRIANGLE)) {
	// 		printf("Stopping big engine!\n");
	// 		paddie.stopBigEngine();
	// 	}
	// 	if (paddie.clicked(PAD_R1)) {
	// 		paddie.startLittleEngine();
	// 		tmp = ("R1 - Start little engine\n");
	// 	}
	// 	if (paddie.clicked(PAD_L1)) {
	// 		paddie.stopLittleEngine();
	// 		tmp = ("L1 - Stop little engine\n");
	// 	}
	// 	if (paddie.clicked(PAD_R2)) {
	// 		tmp = ("R2\n");
	// 	}
	// 	if (paddie.clicked(PAD_L2)) {
	// 		tmp = ("L2\n");
	// 	}

	// 	/* BJ ADDED HERE */
	// 	if (paddie.held(PAD_CROSS)) {
	// 		tmp = ("X\n");
	// 	}

	// 	if (paddie.getPressure(PAD_LEFT)) {
	// 		tmp = ("LEFT\n");
	// 	}



	// 	gsKit_clear(gsGlobal, Grey);
		
	// 	//////
	// 	manny.draw();
	// 	manny.print_title();
	// 	//////

	// 	gsKit_queue_exec(gsGlobal);
	// 	gsKit_sync_flip(gsGlobal);
	// 	gsKit_TexManager_nextFrame(gsGlobal);


	// }

	render(&frame, &z, &texbuff, gsGlobal);


	packet2_free(vif_packets[0]);
	packet2_free(vif_packets[1]);
	packet2_free(purdue_logo_packet);

	// Sleep
	SleepThread();


    // //////////////////////////////////////////////////////////////////
    // Vec4 vectorino = Vec4(0,0,0,1);
    // sleep(3);
    // printf("!!!!!!!!!!!!!!!!!!!%f!!!!!!!!!!!!!!!!!!!\n", vectorino.w);

    return 0;
}