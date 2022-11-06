#include "../include/glmCamera.hpp"

glmCamera::glmCamera()
{

}


glmCamera::glmCamera(Vec3 _position)
{
    position = _position;

    vif_packets[0] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);
	vif_packets[1] = packet2_create(11, P2_TYPE_NORMAL, P2_MODE_CHAIN, 1);

    init_gs();
}


glmCamera::~glmCamera()
{

}

Mat4 glmCamera::M()
{
    glm::mat4 view = glm::mat4(1.0);
	glm::mat4 proj = glm::mat4(1.0);

	view = glm::lookAt(position, position + front, up);
	proj = glm::perspective(glm::radians(fov), (float)width / height, near, far);

    pv = proj * view;

    return pv;
}

void glmCamera::init_gs()
{
    // Define a 32-bit 640x512 framebuffer.
	frame.width = width;
	frame.height = height;
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

void glmCamera::init_drawing_environment()
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

void glmCamera::free_packets()
{
    packet2_free(vif_packets[0]);
    packet2_free(vif_packets[1]);
}