#pragma once

#include <stdint.h>
#include <tamtypes.h>

#include <vector>

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace ps2 {
    bool init();
    void close();

    void startFrame();
    void endFrame();  

    void drawTrianglesWireframe(
        std::vector<glm::vec4> verts, 
        glm::mat4& matTrans,
        glm::vec4 color
    );
} // namespace ps2