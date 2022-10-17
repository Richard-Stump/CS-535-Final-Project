// Rey Gonzalez and Richard Stump
// Final Project - Vector Processing on PS2 Vector Unit 1 (and trivially, on VU0)
// CS53500, Fall 2022 @ Purdue
// ---------------------------
// final_project.c - driver for PS2SDK-driven library (fplib.h)
// --------------------------------------------------

#include "../include/project.hpp"

#include "../include/glm/glm.hpp"

class Camera
{
public:

	float x, y, z;
};

int main()
{
    project_title();

	Camera camera;

	camera.x = 12.0f;

    // The minimum buffers needed for single buffered rendering.
	framebuffer_t frame;
	zbuffer_t z;

	// The data packet.
	packet_t *packet = packet_init(50,PACKET_NORMAL);

	// Init GIF dma channel.
	dma_channel_initialize(DMA_CHANNEL_GIF,NULL,0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	// Init the GS, framebuffer, and zbuffer.
	init_gs(&frame,&z);

	// Init the drawing environment and framebuffer.
	init_drawing_environment(packet,&frame,&z);

	// Render the sample.
	render(packet,&frame);

	// Free the vram.
	graph_vram_free(frame.address);

	// Free the packet.
	packet_free(packet);

	// Disable output and reset the GS.
	graph_shutdown();

	// Shutdown our currently used dma channel.
	dma_channel_shutdown(DMA_CHANNEL_GIF,0);

	// Sleep
	SleepThread();

    return 0;
}