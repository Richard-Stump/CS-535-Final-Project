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


Application* app;

void initialize()
{
  SifInitRpc(0);

  initPs2();

  app = new Application();
}


int main(int argc, char *argv[])
{
  printf("Starting program\n");

  initialize();

  if(!app->initialize(640, 480)) {
    std::cout << "Could not intialize application!" << std::endl;
    return -1;
  }

  printf("Initialized!\n");

  clock_t prevTime = clock();

  while(true) {
    clock_t curTime = clock();
    float deltaTime = float(curTime - prevTime) / CLOCKS_PER_SEC;

    prevTime = curTime;

    printf("update\n");
    app->update(deltaTime);

    //printf("update 2\n");
    app->render();
    ps2ClearScreen();
  }

  printf("Exiting Program");

  return 0;
}
