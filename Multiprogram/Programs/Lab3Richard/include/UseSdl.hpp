#pragma once

void initSdl();
void SdlClearScreen();

void drawTrianglesSDL(
    std::vector<glm::vec4> verts, 
    glm::mat4& matTrans,
    glm::vec4 color);