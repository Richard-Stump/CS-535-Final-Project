#pragma once

#include <stdint.h>
#include <tamtypes.h>

#include <vector>

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

void initPs2();
void ps2ClearScreen();
void ps2DrawTrianglesWireframe(
    std::vector<glm::vec4> verts, 
    glm::mat4& matTrans,
    glm::vec4 color);