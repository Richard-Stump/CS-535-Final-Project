#ifndef MISC_HPP_INCLUDED
#define MISC_HPP_INCLUDED

#define USE_SDL2 0

#define RENDER_TEST 1

struct InputState
{
	bool moveForward;
	bool moveBackward;
	bool turnLeft;
	bool turnRight; 
	bool moveLeft;
	bool moveRight;
	bool shoot;
};

#endif//MISC_HPP_INCLUDED