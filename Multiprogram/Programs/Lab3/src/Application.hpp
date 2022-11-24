#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <vector>
#include <utility>
#include <map>
#include <functional>

#include "Mesh.hpp"
#include "GameObject.hpp"
#include "Misc.hpp"

#include <PS2Pad.hpp>

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

/**
 * This class contains the actual game's code. This contains the game's state
 * so that it is not global for the entire application.
 */
class Application
{
public:	
	Application();

	// This is for any initialization that cannot be done in the constructor.
	// If true is returned, the application succeded to initialize and the program
	// runs as normal. If false is returned, the application failed to initialize
	// and the program exits.
	bool initialize(int width, int height);

	// This function is called once per frame, and returns true if the program
	// should keep running.
	bool update(float deltaTime);

	// This function is called when the window is resized.
	void resize(int width, int height);

	// Called when a key is pressed
	void onKeypress(int key, int scancode, int action, int mods);

	// This function is called once per frame after update(), and should render the
	// scene. 
	void render();

private:
	void loadMeshes();

	void renderObjects(glm::mat4& matProjView);
	void renderColliders(glm::mat4& matProjView);
	void collisionDetection();
	void updateInput();

	bool renderDebugSpheres = false;

	const float		fovY	= glm::radians(60.0f);
	const float		near	= 0.1;
	const float		far		= 100.0f;

	const float		ENEMY_CHANCE	= 0.01f;
	const float		POWERUP_CHANCE	= 0.01f;


	glm::mat4		matProj;

	InputState		inputState;

	std::vector<GameObject*>		objects;
	std::vector<std::pair<std::string, Mesh*>>	meshes;

	Player*							player;

	PS2Pad* pad;

	Mesh colliderMesh;
	Mesh levelMesh;
	Mesh triMesh;
};

#endif//APPLICATION_HPP_INCLUDED