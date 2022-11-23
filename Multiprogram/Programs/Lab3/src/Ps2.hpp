#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace ps2::draw 
{

bool init();
void close();

void beginFrame();
void endFrame();

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color);

} // namespace ps2::draw