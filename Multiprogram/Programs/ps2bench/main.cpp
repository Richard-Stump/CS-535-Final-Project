#include <tamtypes.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graph.h>
#include <draw.h>

#include "menu.hpp"
#include "pad.hpp"
#include "birdy/birdy.hpp"

s32 main(void)
{
	Pad::init();
	Menu::Load();
	Menu::InitGS();
	
	Menu::Loop();

	printf("[ee] ~main()\n");
	SleepThread();
}
