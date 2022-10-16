#include "../include/fplib.h"

void debug_text(int x, int y, const char* string, int t)
{
    SifInitRpc(0);
    init_scr();
    sleep(1);
    scr_setXY(x, y);
    scr_printf(string);

    sleep(t);
}

void project_title()
{
    debug_text(25,10,"Vertex Processing on the Sony PlayStation 2\n\
                         -------------------------------------------\n\
                    CS535 Final Project by Rey Gonzalez and Richard Stump", 7);
}

void init_gs(framebuffer_t *frame, zbuffer_t *z)
{
    // Define a 32-bit 512x512 framebuffer.
	frame->width = 512;
	frame->height = 512;
	frame->mask = 0;
	frame->psm = GS_PSM_32;

	// Switch the mask on for some fun.
	//frame->mask = 0xFFFF0000;

	// Allocate some vram for our framebuffer
	frame->address = graph_vram_allocate(frame->width,frame->height, frame->psm, GRAPH_ALIGN_PAGE);

	// Disable the zbuffer.
	z->enable = 0;
	z->address = 0;
	z->mask = 0;
	z->zsm = 0;

	// Initialize the screen and tie the framebuffer to the read circuits.
	graph_initialize(frame->address,frame->width,frame->height,frame->psm,0,0);
}

void init_drawing_environment(packet_t *packet, framebuffer_t *frame, zbuffer_t *z)
{

	// This is our generic qword pointer.
	qword_t *q = packet->data;

	// This will setup a default drawing environment.
	q = draw_setup_environment(q,0,frame,z);

	// This is where you could add various other drawing environment settings,
	// or keep on adding onto the packet, but I'll stop with the default setup
	// by appending a finish tag.

	q = draw_finish(q);

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data,q - packet->data, 0, 0);

	// Wait until the finish event occurs.
	draw_wait_finish();

}

void draw_single_rect(
                        packet_t* packet,
                        qword_t* q,
                        int xmin,
                        int xmax,
                        int ymin, 
                        int ymax, 
                        int r, 
                        int g, 
                        int b,
                        int a, 
                        float t
                    )
{
    q = packet->data;

    // Wait for our previous dma transfer to end.
    dma_wait_fast();

    // Draw another square on the screen.
    PACK_GIFTAG(q, GIF_SET_TAG(4, 1, 0, 0, 0, 1),GIF_REG_AD);
    q++;
    PACK_GIFTAG(q, GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0), GIF_REG_PRIM);
    q++;
    PACK_GIFTAG(q, GIF_SET_RGBAQ(r, g, b, a, 0x3F800000), GIF_REG_RGBAQ);
    q++;
    PACK_GIFTAG(q, GIF_SET_XYZ((xmin << 4) + large_zero, (ymin << 4) + large_zero, 0), GIF_REG_XYZ2);
    q++;
    PACK_GIFTAG(q, GIF_SET_XYZ((xmax << 4) + large_zero, (ymax << 4) + large_zero, 0), GIF_REG_XYZ2);
    q++;

    q = draw_finish(q);

    // DMA send
    dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data,q - packet->data, 0, 0);

    // Wait until the drawing is finished.
    draw_wait_finish();

    // Now initiate vsync.
    graph_wait_vsync();

    sleep(t);
}

void render(packet_t *packet, framebuffer_t *frame)
{
	// Used for the qword pointer.
	qword_t *q;

	// Since we only have one packet, we need to wait until the dmac is done
	// before reusing our pointer;
	dma_wait_fast();

	q = packet->data;
	q = draw_clear(q,0,0,0,frame->width,frame->height,0,0,0);
	q = draw_finish(q);

	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data, q - packet->data, 0, 0);

	// Wait until the screen is cleared.
	draw_wait_finish();

	// Update the screen.
	graph_wait_vsync();

    // blue rectangle test
    draw_single_rect(packet, q, 0, 640, 0, 510, 0, 0, 255, 255, 3);

}