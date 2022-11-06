#include "../include/Camera.hpp"

Camera::Camera(unsigned int w, unsigned int h, Vec3 p)
{
	width = w;
	height = h;
	position = p;

	front = Vec3(0, 0, -1);
	up = Vec3(0, 1, 0);

	move_speed = 0.25f;
	time_speed = delta;

	//bg_color = {0.20, 0.25, 0.30, 0.50};
}

Camera::~Camera()
{

}

void Camera::ins(PS2Pad* gamepad)
{
	gamepad->read();
	// gamepad.
	// const char* tmp = nullptr;


	if(gamepad->b_read()) printf("gamepad!!!!!!!!!!!!!!!!!!!!!!!!\n");
	// // {
	// if (gamepad->held(PAD_DOWN)) {
	// 		printf("DOWN\n");
	// 	}
	// if (gamepad->held(PAD_RIGHT)) {
	// 		printf("RIGHT\n");
	// 		/*
	// 		   padSetMainMode(port, slot,
	// 		   PAD_MMODE_DIGITAL, PAD_MMODE_LOCK));
	// 		 */
	// 	}
	// if (gamepad.clicked(PAD_UP)) {
	// 	tmp = ("UP\n");
	// }
	// if (gamepad.clicked(PAD_START)) {
	// 	tmp = ("START\n");
	// }
	// if (gamepad.clicked(PAD_R3)) {
	// 	tmp = ("R3\n");
	// }
	// if (gamepad.clicked(PAD_L3)) {
	// 	tmp = ("L3\n");
	// }
	// if (gamepad.clicked(PAD_SELECT)) {
	// 	tmp = ("SELECT\n");
	// }
	// if (gamepad.clicked(PAD_SQUARE)) {
	// 	tmp = ("SQUARE\n");
	// 	printf("Starting big engine!\n");
	// 	gamepad.startBigEngine(255);
	// }
	// if (gamepad.clicked(PAD_CROSS)) {
	// 	gamepad.enterPressureMode();
	// 	tmp = ("CROSS - Enter press mode\n");
	// }
	// if (gamepad.clicked(PAD_CIRCLE)) {
	// 	gamepad.exitPressureMode();
	// 	tmp = ("CIRCLE - Exit press mode\n");
	// }
	// if (gamepad.clicked(PAD_TRIANGLE)) {
	// 	printf("Stopping big engine!\n");
	// 	gamepad.stopBigEngine();
	// }
	// if (gamepad.clicked(PAD_R1)) {
	// 	gamepad.startLittleEngine();
	// 	tmp = ("R1 - Start little engine\n");
	// }
	// if (gamepad.clicked(PAD_L1)) {
	// 	gamepad.stopLittleEngine();
	// 	tmp = ("L1 - Stop little engine\n");
	// }
	// if (gamepad.clicked(PAD_R2)) {
	// 	tmp = ("R2\n");
	// }
	// if (gamepad.clicked(PAD_L2)) {
	// 	tmp = ("L2\n");
	// }

	// /* BJ ADDED HERE */
	// if (gamepad.held(PAD_CROSS)) {
	// 	tmp = ("X\n");
	// }
	// if (gamepad.getPressure(PAD_LEFT)) {
	// 	tmp = ("LEFT\n");
	// }
	// }

	// printf("%s", tmp);
}

// void Camera::M(float fov, float near, float far, Shader& shader, const char* variable)
// {
// 	glm::mat4 view = glm::mat4(1.0);
// 	glm::mat4 proj = glm::mat4(1.0);

// 	view = glm::lookAt(position, position + front, up);
// 	proj = glm::perspective(glm::radians(fov), (float)width / height, near, far);

// 	glUniformMatrix4fv(glGetUniformLocation(shader.ID, variable), 1, GL_FALSE, glm::value_ptr(proj * view));
// }

void Camera::clear()
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

void Camera::boundary()
{
	if (position.x < x_min) position.x = x_min;
	if (position.x > x_max) position.x = x_max;
	if (position.y < y_min) position.y = y_min;
	if (position.y > y_max) position.y = y_max;
	if (position.z < z_min) position.z = z_min;
	if (position.z > z_max) position.z = z_max;
}

void Camera::set_bounds(float _x_min, float _x_max, float _y_min, float _y_max, float _z_min, float _z_max)
{
	x_min = _x_min;
	x_max = _x_max;
	y_min = _y_min;
	y_max = _y_max;
	z_min = _z_min;
	z_max = _z_max;
}

void Camera::set_bounds(float boundary_array[6])
{
	x_min = boundary_array[0];
	x_max = boundary_array[1];
	y_min = boundary_array[2];
	y_max = boundary_array[3];
	z_min = boundary_array[4];
	z_max = boundary_array[5];
}

void Camera::step()
{
	t += time_speed;
}

void Camera::init_gs()
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

	graph_initialize(frame.address, frame.width, frame.height, frame.psm, 0, 0);
}

void Camera::init_drawing_environment()
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

void Camera::M(float fov, float near, float far)
{
	// glm::mat4 view = glm::mat4(1.0);
	// glm::mat4 proj = glm::mat4(1.0);
	// glm::mat4 screen = glm::mat4(1.0);
	// float n_fov = fov * M_PI / 180.0f;

	world_view = glm::lookAt(position, position + front, up);
	view_screen = glm::perspective(glm::radians(fov), (float)width / height, near, far);
	A = world_view * view_screen;
}