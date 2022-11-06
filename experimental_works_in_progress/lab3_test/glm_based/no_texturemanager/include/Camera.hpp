// heavily inspired by Victor Gordan's work

#pragma once
#include "../../../globals/include/glm/glm.hpp"
#include "../../../globals/include/glm/gtc/matrix_transform.hpp"
#include "../../../globals/include/glm/gtc/type_ptr.hpp"
#include "../../../globals/include/glm/gtx/rotate_vector.hpp"
#include "../../../globals/include/glm/gtx/vector_angle.hpp"

#include "../../../globals/include/PS2Pad.hpp"


// #include "../include/main.h"
using Color3 = glm::vec3;
using Color4 = glm::vec4;
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Mat3 = glm::mat3;

#include <kernel.h>
#include <stdio.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>
#include <gif_tags.h>

#include <gs_gp.h>
#include <gs_psm.h>

#include <dma.h>
#include <dma_tags.h>
#include <packet2.h>
#include <packet2_utils.h>

#include <draw.h>
#include <graph.h>
#include <packet.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef GRAVITY
#define GRAVITY 9.7705078125
#endif

class Camera
{
public:
	Camera(unsigned int w, unsigned int h, Vec3 p);

	~Camera();

	void ins(PS2Pad* gamepad);

	// void M(float fov, float near, float far, Shader& shader, const char* variable);

	void clear();

	void boundary();

	void set_bounds(float _x_min, float _x_max, float _y_min, float _y_max, float _z_min, float _z_max);
	void set_bounds(float boundary_array[6]);

	void step();
	void init_gs();
	void init_drawing_environment();
	void M(float fov, float near, float far);

	//void boundary(float x_bound_low, float x_bound_high, float y_bound_low, float y_bound_high);

	//void boundary(const char* one, const char* two);

public:
	unsigned int 	width,
					height;

	float 	t = 0.0f,
			delta = 0.01f;

	Vec3 	position,
			rotation,
			front,
			up;

	Mat4	world_view,
			view_screen,
			A;

	float 	move_speed,
			time_speed;

	Color4 bg_color = { 0.20, 0.25, 0.30, 0.50 };

	float 	x_min, x_max,
			y_min, y_max,
			z_min, z_max;

	framebuffer_t frame;
	zbuffer_t z;
};