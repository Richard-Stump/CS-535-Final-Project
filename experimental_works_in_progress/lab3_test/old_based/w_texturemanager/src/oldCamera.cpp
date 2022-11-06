#include "../include/oldCamera.hpp"

void oldCamera::init_gs()
{
	// Define a 32-bit 640x512 framebuffer.
	frame.width = 640;
	frame.height = 512;
	frame.mask = 0;
	frame.psm = GS_PSM_32;
	frame.address = graph_vram_allocate(frame.width, frame.height, frame.psm, GRAPH_ALIGN_PAGE);

	// Enable the zbuffer.
	z.enable = DRAW_ENABLE;
	z.mask = 0;
	z.method = ZTEST_METHOD_GREATER_EQUAL;
	z.zsm = GS_ZBUF_32;
	z.address = graph_vram_allocate(frame.width, frame.height, z.zsm, GRAPH_ALIGN_PAGE);

	// Initialize the screen and tie the first framebuffer to the read circuits.
	graph_initialize(frame.address, frame.width, frame.height, frame.psm, 0, 0);
}

void oldCamera::init_drawing_environment()
{
    packet2_t *packet2 = packet2_create(20, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// This will setup a default drawing environment.
	packet2_update(packet2, draw_setup_environment(packet2->next, 0, &frame, &z));

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	packet2_update(packet2, draw_primitive_xyoffset(packet2->next, 0, (2048 - 320), (2048 - 256)));

	// Finish setting up the environment.
	packet2_update(packet2, draw_finish(packet2->next));

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
	dma_wait_fast();

	packet2_free(packet2);
}

oldCamera::oldCamera()
{
	// position = {140.00f, 140.00f, 320.00f, 1.00f};
	// position[0] = 140.00f;
	// position[1] = 140.00f;
	// position[2] = 320.00f;
	// position[3] = 1.00f;

	rotation[0] = 0.00f;
	rotation[1] = 0.00f;
	rotation[2] = 0.00f;
	rotation[3] = 1.00f;

    vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	init_gs();
	// init_drawing_environment();
}

void oldCamera::free_packets()
{
    packet2_free(vif_packets[0]);
	packet2_free(vif_packets[1]);
}

void oldCamera::clear()
{
	packet2_t *clear = packet2_create(35, P2_TYPE_NORMAL, P2_MODE_NORMAL, 0);

	// Clear framebuffer but don't update zbuffer.
	packet2_update(clear, draw_disable_tests(clear->next, 0, &z));
	packet2_update(clear, draw_clear(clear->next, 0, 2048.0f - 320.0f, 2048.0f - 256.0f, frame.width, frame.height, 0x40, 0x40, 0x40));
	packet2_update(clear, draw_enable_tests(clear->next, 0, &z));
	packet2_update(clear, draw_finish(clear->next));

	// Now send our current dma chain.
	dma_wait_fast();
	dma_channel_send_packet2(clear, DMA_CHANNEL_GIF, 1);

	packet2_free(clear);

	// Wait for scene to finish drawing
	draw_wait_finish();
}

oldCamera::oldCamera(float a, float b, float c, float d)
{
	position[0] = a;
	position[1] = b;
	position[2] = c;
	position[3] = d;

	rotation[0] = 0.00f;
	rotation[1] = 0.00f;
	rotation[2] = 0.00f;
	rotation[3] = 1.00f;

    vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

	init_gs();
}

void oldCamera::boundary(float xmin, float xmax, float zmin, float zmax)
{
	if(position[0] < xmin) position[0] = xmin;
	if(position[0] > xmax) position[0] = xmax;
	if(position[2] < zmin) position[2] = zmin;
	if(position[2] > zmax) position[2] = zmax;
}

void oldCamera::boundary(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
	if(position[0] < xmin) position[0] = xmin;
	if(position[0] > xmax) position[0] = xmax;
	if(position[1] < ymin) position[1] = ymin;
	if(position[1] > ymax)
	{
		if(falling)
		{
			falling = false;
			current_jump_speed = 0;

			pre_jump_speed_vector[0] = 0;
			pre_jump_speed_vector[2] = 0;
		}
	}
	if(position[2] < zmin) position[2] = zmin;
	if(position[2] > zmax) position[2] = zmax;
	if(falling) current_jump_speed -= gravity_per_frame;
}

// void oldCamera::update()
// {
// 	if(position[1] < 0) position[1] -= 9.8/1000.0f;
// 	else position[1] = 0;
// }

void oldCamera::set_pre_jump_speed(float x, float z)
{
	pre_jump_speed_vector[0] = x;
	pre_jump_speed_vector[1] = z;
}