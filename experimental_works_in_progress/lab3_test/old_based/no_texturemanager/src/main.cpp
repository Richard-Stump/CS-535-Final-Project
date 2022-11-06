#include "../include/main.hpp"
// extern unsigned char bedrichAlpha0[];

extern u32 VU1Draw3D_CodeStart __attribute__((section(".vudata")));
extern u32 VU1Draw3D_CodeEnd __attribute__((section(".vudata")));


/** Calculate cube position and add packet with cube data */
void draw_cube(VECTOR* c_verts, VECTOR* c_sts, oldCamera& cam, TexturedCubeMesh& cube)
{
	create_local_world(cube.model, cube.position, cube.rotation);
	create_world_view(cam.world_view, cam.position, cam.rotation);
	create_local_screen(cam.local_screen, cube.model, cam.world_view, cam.view_screen);
	cam.curr_vif_packet = cam.vif_packets[cam.context];
	packet2_reset(cam.curr_vif_packet, 0);

	// Add matrix at the beggining of VU mem (skip TOP)
	packet2_utils_vu_add_unpack_data(cam.curr_vif_packet, 0, &cam.local_screen, 8, 0);

	u32 vif_added_bytes = 0; // zero because now we will use TOP register (double buffer)
							 // we don't wan't to unpack at 8 + beggining of buffer, but at
							 // the beggining of the buffer

	// Merge packets
	packet2_utils_vu_add_unpack_data(cam.curr_vif_packet, vif_added_bytes, cube.packet->base, packet2_get_qw_count(cube.packet), 1);
	vif_added_bytes += packet2_get_qw_count(cube.packet);

	// Add vertices
	packet2_utils_vu_add_unpack_data(cam.curr_vif_packet, vif_added_bytes, c_verts, cube.face_count, 1);
	vif_added_bytes += cube.face_count; // one VECTOR is size of qword

	// Add sts
	packet2_utils_vu_add_unpack_data(cam.curr_vif_packet, vif_added_bytes, c_sts, cube.face_count, 1);
	vif_added_bytes += cube.face_count;

	packet2_utils_vu_add_start_program(cam.curr_vif_packet, 0);
	packet2_utils_vu_add_end_tag(cam.curr_vif_packet);
	dma_channel_wait(DMA_CHANNEL_VIF1, 0);
	dma_channel_send_packet2(cam.curr_vif_packet, DMA_CHANNEL_VIF1, 1);

	// Switch packet, so we can proceed during DMA transfer
	cam.context = !cam.context;
}

void align_data_arrays(VECTOR* c_verts, VECTOR* c_sts, TexturedCubeMesh& cube)
{
	int i, j;
	for (i = 0; i < cube.face_count; i++)
	{
		c_verts[i][0] = cube.vertices[cube.faces[i]][0];
		c_verts[i][1] = cube.vertices[cube.faces[i]][1];
		c_verts[i][2] = cube.vertices[cube.faces[i]][2];
		c_verts[i][3] = cube.vertices[cube.faces[i]][3];

		c_sts[i][0] = cube.sts[cube.faces[i]][0];
		c_sts[i][1] = cube.sts[cube.faces[i]][1];
		c_sts[i][2] = cube.sts[cube.faces[i]][2];
		c_sts[i][3] = cube.sts[cube.faces[i]][3];
	}
}

void render(oldCamera& cam, TexturedCubeMesh& cube, PS2Pad& gamepad)
{
	// float rotation_sensitivity = 0.0005f;
	// float walk_sensitivity = 0.005f;
	create_view_screen(cam.view_screen, graph_aspect_ratio(), -10.00f, 10.00f, -10.00f, 10.00f, 1.00f, 160.00f);
	
	// int i, j;

	cube.set_lod_clut_prim_tex_buff();

	/** 
	 * Allocate some space for object position calculating. 
	 * c_ prefix = calc_
	 */
	VECTOR* c_verts = (VECTOR *)memalign(128, sizeof(VECTOR) * cube.face_count);
	VECTOR* c_sts = (VECTOR *)memalign(128, sizeof(VECTOR) * cube.face_count);

	align_data_arrays(c_verts, c_sts, cube);
	
	cube.calculate();
	float pad_low_pass = 64;

	// The main loop...
	for (;;)
	{
		float up[3] = {0,1,0};
		float front[3] = {sin(cam.rotation[1]), 0, cos(cam.rotation[1])};
		VECTOR left;

		vector_cross_product(left, up, front);
		// cam.update();
		cam.boundary(-100.0f, 100.0f, -100, 0, -100.0f, 100.0f);
		gamepad.read();

		float rightxdiff = gamepad.getRightJoyX() - 127.0f;
		float rightydiff = gamepad.getRightJoyY() - 127.0f;
		float leftydiff = gamepad.getLeftJoyY() - 127.0f;
		float leftxdiff = gamepad.getLeftJoyX() - 127.0f;
		
		float xmove, zmove, factor1, factor2;

		if(!cam.falling)
		{
			xmove = 0, zmove = 0;
			factor1 = 24 * cam.walk_sensitivity * cam.walking_speed_per_second * leftydiff;
			factor2 = cam.walk_sensitivity * leftxdiff;

			xmove += (front[0] * factor1) + (left[0] * factor2);
			zmove += (front[2] * factor1) + (left[2]  * factor2);

			if(gamepad.held(PAD_CROSS))
			{
				cam.falling = true;
				cam.current_jump_speed = cam.standing_jump_speed;
				cam.pre_jump_speed_vector[0] = xmove;
				cam.pre_jump_speed_vector[2] = zmove;
			}

			cam.position[0] += xmove;
			cam.position[2] += zmove;
		}

		if(rightxdiff < -pad_low_pass || rightxdiff > pad_low_pass) cam.rotation[1] -= cam.rotation_sensitivity * rightxdiff;
		if(rightydiff < -pad_low_pass || rightydiff > pad_low_pass) cam.rotation[0] = 4 * cam.rotation_sensitivity * rightydiff;
		else cam.rotation[0] = 0;
		
		if(cam.falling)
		{
			cam.position[0] += cam.pre_jump_speed_vector[0];
			cam.position[1] -= cam.current_jump_speed;
			cam.position[2] += cam.pre_jump_speed_vector[2];
		}

		cam.clear();

		draw_cube(c_verts, c_sts, cam, cube);

		graph_wait_vsync();
	}
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

int main(int argc, char *argv[])
{
	// Init DMA channels.
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_initialize(DMA_CHANNEL_VIF1, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);

	vu1_upload_micro_program();
	vu1_set_double_buffer_settings();

	oldCamera cam(0,2,20,1);
	TexturedCubeMesh cube(0,1,0,1);
	PS2Pad gamepad;

	gamepad.init(0,0);
	cam.init_drawing_environment();



	unsigned char textito[16] = {
		128, 0, 0, 0,
		128, 0, 0, 0,
		128, 0, 0, 0,
		128, 0, 0, 0
	};
	cube.send_texture(textito);

	render(cam, cube, gamepad);

	cam.free_packets();
	cube.free_packet();

	// Sleep
	SleepThread();

	// End program.
	return 0;
}