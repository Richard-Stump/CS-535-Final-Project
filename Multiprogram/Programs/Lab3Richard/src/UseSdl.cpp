#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <vector>

#include "Misc.hpp"

#if USE_SDL2

static	SDL_Window* 	window;
static	SDL_Surface* 	surface;

void SdlClearScreen() {
    SDL_Event event;
	while(SDL_PollEvent(&event) != 0) {}

	SDL_UpdateWindowSurface(window);

	SDL_Rect rect = {0, 0, 640, 480};
	SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 32, 32, 32));
}

static void plot_line (int x0, int y0, int x1, int y1, uint32_t color)
{
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; /* error value e_xy */
 
  for (;;){  /* loop */
	{
		SDL_Rect r = {x0, y0, 1, 1};
		SDL_FillRect(surface, &r, color);
	}

    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}

void drawTrianglesSDL(
    std::vector<glm::vec4> verts, 
    glm::mat4& matTrans,
    glm::vec4 color)
{
	uint32_t scolor = SDL_MapRGB(surface->format,
		(uint8_t)(color.r * 255.0f), 
		(uint8_t)(color.g * 255.0f), 
		(uint8_t)(color.b * 255.0f)
	);

	for(int i = 0; i < verts.size(); i += 3) {
		glm::vec4 v1 = verts[i];
		glm::vec4 v2 = verts[i + 1];
		glm::vec4 v3 = verts[i + 2];

		// Transform
		v1 = matTrans * v1;
		v2 = matTrans * v2;
		v3 = matTrans * v3;

        if(
            v1.w <= 0.0f || 
            v2.w <= 0.0f ||
            v3.w <= 0.0f
        ) {
            continue;
        }

		// Perspective divide
		v1 = v1 / v1.w;
		v2 = v2 / v2.w;
		v3 = v3 / v3.w;

		if(
			v1.x < -1.0f || v1.x > 1.0f ||
			v1.y < -1.0f || v1.y > 1.0f ||
			v2.x < -1.0f || v2.x > 1.0f ||
			v2.y < -1.0f || v2.y > 1.0f ||
			v3.x < -1.0f || v3.x > 1.0f ||
			v3.y < -1.0f || v3.y > 1.0f
		) {
			continue;
		}

		// Viewport transform
		int x1 = (int)((v1.x + 1.0f)  * 640.0f / 2.0f);
		int x2 = (int)((v2.x + 1.0f)  * 640.0f / 2.0f);
		int x3 = (int)((v3.x + 1.0f)  * 640.0f / 2.0f);

		int y1 = (int)((1.0f - v1.y)  * 480.0f / 2.0f);
		int y2 = (int)((1.0f - v2.y)  * 480.0f / 2.0f);
		int y3 = (int)((1.0f - v3.y)  * 480.0f / 2.0f);

		plot_line(x1, y1, x2, y2, scolor);
		plot_line(x2, y2, x3, y3, scolor);
		plot_line(x3, y3, x1, y1, scolor);
	}
}

void initSdl() {
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Could not initialize SDL2!\n     SDL_Error: %s\n", SDL_GetError());
	}
	else {
		printf("SDL2 Initialized!\n    SDL_Error: %s\n", SDL_GetError());
	}
	window = SDL_CreateWindow("CHEAT", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		640, 480,
		0
	);
	if(window == nullptr) {
		printf("Could not create window\n   SDL_Error: %s\n", SDL_GetError());
	}

	surface = SDL_GetWindowSurface(window);

	printf("Initialized SDL2!\n");

	//SDL_InitSubSystem(SDL_INIT_JOYSTICK);
}
#endif