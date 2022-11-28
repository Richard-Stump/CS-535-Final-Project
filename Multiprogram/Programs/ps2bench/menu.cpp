#include "menu.hpp"

Menu::MenuObject Menu::topLevel;
Menu::MenuObject* Menu::currentLevel;
extern GSGLOBAL* gsGlobal; // HACK
// Use this for categories that don't rely on
// custom click functionality
// Settings the clickHandler object to 0x0 will crash!
void handlerNULL(u32 sel){};

void handlerEEBenchArith(u32 ignored)
{
	eebench_loopArithmetic();
}

void handlerEEBenchArith2(u32 ignored)
{
	eebench_dwArithmetic();
}

void handlerEEBenchSmallBlock(u32 ignored)
{
	eebench_smallBlocks();
}

void handlerEEBenchSDXLDX(u32 ignored)
{
	eebench_SDXLDXFallback();
}

void handlerEEMiscRecKiller(u32 sel)
{
	eemisc_recKiller();
}

void handlerVUBenchGeneric(u32 sel)
{
	vubench_generic();
}

void handlerVUBenchRegpressure(u32 sel)
{
	vubench_registerPressure();
}

void handlerVUMiscIBit(u32 sel)
{
	vumisc_IBitRecompilation();
}

void handlerGSmppsSprite(u32 sel)
{
	gs_mpps_sprite();
}

void handlerGSMaxPoly(u32 sel)
{
	gs_maxpoly(false);
}

void handlerGSMaxPolyUnsafe(u32 sel)
{
	gs_maxpoly(true);
}

void handlerBirdyVU1(u32 sel)
{
	Birdy::VU1::Bench(gsGlobal);
}

void handlerAbout(u32 sel)
{
}

void handlerSlides(u32 sel)
{

}

void handlerLab3(u32 sel)
{

}

void handlerBedrich(u32 sel)
{

}

void handlerPurdue(u32 sel)
{

}

void handlerControllerCube(u32 sel)
{

}

void Menu::Load()
{
	printf("[ee] Starting menu load\n");
	currentLevel = &topLevel;
	topLevel.parent = &topLevel;
	topLevel.clickHandler = handlerNULL;
	topLevel.title = "CS535 Final Project Presentation\n";
	
	MenuObject* slides = new MenuObject(&topLevel, ObjectType::FUNCTION, "Start Slides", handlerSlides);

	MenuObject* lab3 = new MenuObject(&topLevel, ObjectType::FUNCTION, "Lab 3", handlerLab3);

	MenuObject* catCubes = new MenuObject(&topLevel, ObjectType::CATEGORY, "Many Cubes", handlerNULL);
	{
		MenuObject* cubePurdue = new MenuObject(catCubes, ObjectType::FUNCTION, "Purdue", handlerPurdue);
		MenuObject* cubeBedrich = new MenuObject(catCubes, ObjectType::FUNCTION, "Bedrich", handlerBedrich);

	}
	MenuObject* controllerCube = new MenuObject(&topLevel, ObjectType::FUNCTION, "Controller Cube", handlerControllerCube);



	/*
	MenuObject* catEE = new MenuObject(&topLevel, ObjectType::CATEGORY, "CPU", handlerNULL);
	{

		MenuObject* catEEBench = new MenuObject(catEE, ObjectType::CATEGORY, "CPU Demos", handlerNULL);
		{
			new MenuObject(catEEBench, ObjectType::FUNCTION, "Primitives", handlerEEBenchArith);

			new MenuObject(catEEBench, ObjectType::FUNCTION, "Movement", handlerEEBenchArith2);

			new MenuObject(catEEBench, ObjectType::FUNCTION, "3D", handlerEEBenchSmallBlock);
		}

		MenuObject* catEEMisc = new MenuObject(catEE, ObjectType::CATEGORY, "CPU Misc", handlerNULL);
		{
			new MenuObject(catEEMisc, ObjectType::FUNCTION, "Surprise!", handlerEEMiscRecKiller);
		}
	}

	MenuObject* catVU = new MenuObject(&topLevel, ObjectType::CATEGORY, "VU1", handlerNULL);
	{
		MenuObject* catVUBench = new MenuObject(catVU, ObjectType::CATEGORY, "VU Demos", handlerNULL);
		{
			new MenuObject(catVUBench, ObjectType::FUNCTION, "Single Color Primitives", handlerVUBenchGeneric,true);

			new MenuObject(catVUBench, ObjectType::FUNCTION, "Textured Primitives", handlerVUBenchRegpressure,true);

			new MenuObject(catVUBench, ObjectType::FUNCTION, "Basic 3D Camera and Movement", handlerVUBenchRegpressure,true);
		}

		MenuObject* catVUMisc = new MenuObject(catVU, ObjectType::CATEGORY, "VU Misc", handlerNULL);
		{
			new MenuObject(catVUMisc, ObjectType::FUNCTION, "Surprise!", handlerVUMiscIBit,true);
		}
	}

	MenuObject* catGS = new MenuObject(&topLevel, ObjectType::CATEGORY, "GS Demos", handlerNULL);
	{
		new MenuObject(catGS, ObjectType::FUNCTION, "?", handlerGSmppsSprite, true);
		new MenuObject(catGS, ObjectType::FUNCTION, "?", handlerGSMaxPoly, true);
		new MenuObject(catGS, ObjectType::FUNCTION, "?", handlerGSMaxPolyUnsafe, true);
	}

	// MenuObject* catBirdy = new MenuObject(&topLevel,ObjectType::CATEGORY, "Birdy", handlerNULL);
	// {
	// 	new MenuObject(catBirdy, ObjectType::FUNCTION,"VU1",handlerBirdyVU1,true);
	// }
	*/

	new MenuObject(&topLevel, ObjectType::FUNCTION, "About", handlerAbout);
	printf("[ee] Finished menu load!\n");
	return;
}
