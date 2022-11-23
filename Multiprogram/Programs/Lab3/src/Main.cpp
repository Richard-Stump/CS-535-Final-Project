#include <stdio.h>
#include <vector>
#include <time.h>

#include <glm/glm.hpp>

#include "Ps2.hpp"
#include "Application.hpp"

using namespace ps2;

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color)
{
    draw::drawTrianglesWireframe(verts, matTrans, color);
}

int main(int argc, char** argv)
{
    printf("Beginning Lab3\n");

    printf("Initializing drawing environment\n");
    if(!draw::init()) {
        printf("    Error: Could not initialize\n");
        return -1;
    }
    else {
        printf("    Done!\n");
    }

    printf("Initializing application\n");
    Application* app = new Application();
    if(app == nullptr) {
        printf("    Could not allocate memory for application\n");
        return -1;
    }
    if(!app->initialize(640, 480)) {
        printf("    App failed to initialize\n" );
        return -1;
    }
    printf("    Done!\n");

    printf("Entering main loop\n");

    clock_t prevTime = clock();
    bool running = true;
    while(running) {
        clock_t curTime = clock();
        float deltaTime = float(curTime - prevTime) / CLOCKS_PER_SEC;
        prevTime = curTime;
        
        // Update the gamestate
        app->update(deltaTime);

        draw::beginFrame();
        app->render();
        draw::endFrame();
    }

    printf("Returning From Lab3\n");
}