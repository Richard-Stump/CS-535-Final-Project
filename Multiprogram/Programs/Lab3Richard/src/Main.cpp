/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
*/

#include <stdio.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>
#include <graph.h>
#include <time.h>

#include <iostream>
#include <fstream>

#include <PS2Pad.hpp>

#include "Mesh.hpp"
#include "Ps2.hpp"

#include "Application.hpp"

#include <chrono>

namespace draw = ps2;

Application* app;

void drawTrianglesWireframe(
	std::vector<glm::vec4> verts, 
	glm::mat4& matTrans,
	glm::vec4 color
)
{
  draw::drawTrianglesWireframe(verts, matTrans, color);
}

int main(int argc, char *argv[])
{
  std::cout << "Start of Lab3\n" << std::endl;

  std::cout << "Initializing Drawing Environment:\n";
  if(!draw::init()) {
    std::cout << "    Could not initialize drawing environment!" << std::endl;
    return -1;
  }
  else 
    std::cout << "    Done!" << std::endl;
    
  std::cout << "Allocating Application:\n";
  app = new Application();
  if(app == nullptr) {
    std::cout << "    Could not allocate memory for application!\n" << std::endl;
    return -1;
  }
  else
    std::cout << "    Done!" << std::endl;


  std::cout << "Initializing Application:\n";
  if(!app->initialize(640, 512)) {
    std::cout << "    Could not intialize application!" << std::endl;
    return -1;
  }
  else
    std::cout << "    Done!" << std::endl;

  printf("Initialized!\n");

  clock_t prevTime = clock();
  while(true) {
    // Calculate the change in time since the previous frame
    std::cout << "Update" << std::endl;
    clock_t curTime = clock();
    float deltaTime = float(curTime - prevTime) / CLOCKS_PER_SEC;
    prevTime = curTime;
    
    // Update the gamestate
    app->update(deltaTime);

    // Render
    draw::startFrame();
    app->render();
    draw::endFrame();
  }

  printf("Exiting Program");

  return 0;
}
