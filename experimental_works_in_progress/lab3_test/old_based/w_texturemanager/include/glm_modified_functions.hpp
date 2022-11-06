#pragma once

#include "../../globals/include/glm/glm.hpp"

using Vec4 = glm::vec4;
using Vec3 = glm::vec3;
using Mat4 = glm::mat4;

Mat4 model_gen(Mat4, Vec4, Vec4);
Mat4 model_gen(Mat4, Vec3, Vec3);