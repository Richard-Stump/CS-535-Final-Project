//=============================================================================
// CS 535 - Lab03
// Richard Stump
// 
// Application.cpp: Code for implementing the game
//=============================================================================

#include "Application.hpp"

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>

#include <PS2Pad.hpp>

#include "Mesh.hpp"
#include "GameObject.hpp"


#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>

Application::Application()
{
	loadMeshes();

	player = new Player();
	if(player == nullptr) {
		printf("COULD NOT LOAD PLAYER!!!!\n");
	}
	player->pos.x = -10;

	objects.push_back(player);

	for (int y = 0; y < ROOM_HEIGHT; y++) {
		for (int x = 0; x < ROOM_WIDTH; x++) {
			float enemyChance = glm::linearRand(0.0f, 1.0f);
			float powerupChance = glm::linearRand(0.0f, 1.0f);

			float sx = x - ROOM_WIDTH / 2.2f;
			float sy = y - ROOM_HEIGHT / 2.2f;

			if (enemyChance <= ENEMY_CHANCE)
				objects.push_back(new Enemy(sx, sy));
			if (powerupChance <= POWERUP_CHANCE)
				objects.push_back(new Powerup(sx, sy));
		}
	}
}

bool Application::initialize(int width, int height)
{
	resize(width, height);
	
#if USE_SDL2

#else
	// pad.init(0,0);
#endif

	return true;
}

void Application::updateInput() {
#if USE_SDL2
#else
	// pad.b_read();

	// inputState.moveBackward = pad.held(PAD_DOWN);
	// inputState.moveForward = pad.held(PAD_UP);
	// inputState.moveLeft = pad.held(PAD_LEFT);
	// inputState.moveRight = pad.held(PAD_RIGHT);
	// inputState.turnLeft = pad.held(PAD_SQUARE);
	// inputState.turnRight = pad.held(PAD_CIRCLE);

	// inputState.shoot = pad.held(PAD_CROSS);
#endif
}

bool Application::update(float deltaTime)
{
	updateInput();

	for (GameObject* object : objects)
		object->update(deltaTime, inputState);

	collisionDetection();

	// sweep through the object list and delete any objects that are marked
	// for deletion and spawn any new objects
	for (int i = 0; i < objects.size(); ) {
		GameObject* toSpawn = objects[i]->getObjectToSpawn();
		if (toSpawn != nullptr)
			objects.push_back(toSpawn);

		if (objects[i]->shouldDelete()) {
			delete objects[i];
			objects.erase(objects.begin() + i);
		}
		else
			i++;
	}

	return true;
}

void Application::collisionDetection()
{
	//For each pair of objects
	for (int i = 0; i < objects.size(); i++) {
		for (int j = i + 1; j < objects.size(); j++) {
			GameObject* a = objects[i];
			GameObject* b = objects[j];

			float distance = glm::distance(a->pos, b->pos);

			if (distance < a->radius() + b->radius()) {
				a->onCollision(*b);
				b->onCollision(*a);
			}
		}
	}
}

void Application::onKeypress(int key, int scancode, int action, int mods)
{
	//auto updateBool = [=](bool curState) -> bool {
	//auto updateBool = [=](bool curState) -> bool {
	//	if (action == GLFW_PRESS)
	//		return true;
	//	else if (action == GLFW_RELEASE)
	//		return false;
	//	else
	//		return curState;
	//};

// Don't do this, this is here for rapid prototyping
//#define CASE_KEY_STATE(key, state, func) \
//	case key:			\
//	state = func(state);	\
//	break;
//
//	switch (key) {
//		CASE_KEY_STATE(GLFW_KEY_W, inputState.moveForward, updateBool);
//		CASE_KEY_STATE(GLFW_KEY_S, inputState.moveBackward, updateBool);
//		CASE_KEY_STATE(GLFW_KEY_LEFT, inputState.turnLeft, updateBool);
//		CASE_KEY_STATE(GLFW_KEY_RIGHT, inputState.turnRight, updateBool);
//		CASE_KEY_STATE(GLFW_KEY_A, inputState.moveLeft, updateBool);
//		CASE_KEY_STATE(GLFW_KEY_D, inputState.moveRight, updateBool);
//		CASE_KEY_STATE(GLFW_KEY_SPACE, inputState.shoot, updateBool);
//	}
//#undef CASE_KEY_STATE
//
//	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
//		renderDebugSpheres = !renderDebugSpheres;
//	}
}

void Application::resize(int width, int height)
{
	//glViewport(0, 0, width, height);

	float aspect = (double)width / (double)height;

	matProj = glm::perspective(fovY, aspect, near, far);
}

void Application::render()
{
	glm::mat4 matView = player->getViewMatrix();
	glm::mat4 matProjView = matProj * matView;

#if RENDER_TEST
	glm::mat4 id(1.0);
	triMesh.draw(id, {1.0, 0.0, 0.0});
#else

	/* Currently the level mesh is too large to render */
	//levelMesh.draw(matProjView, glm::vec3{1, 1, 1});

	/* The collider mesh is too large to render */
	//if (renderDebugSpheres)
	//	renderColliders(matProjView);
	
	renderObjects(matProjView);
#endif
}

void Application::renderColliders(glm::mat4& matProjView)
{
	//program.use();
	//checkGl();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (GameObject* object : objects) {
		// The player doesn't get rendered
		if (object->type() == player->type())
			continue;

		float radius = object->radius();

		glm::mat4 matModel = glm::translate(object->pos) 
			* glm::scale(glm::vec3{radius, radius, radius});

		glm::mat4 matTrans = matProjView * matModel;

		// program.setVec3("color", glm::vec3{0.0f, 0.5f, 0.5f});
		// program.setMat4("matTrans", matTrans);

		colliderMesh.draw(matTrans, glm::vec3{0.0f, 0.5f, 0.5f});
	}
}

void Application::renderObjects(glm::mat4& matProjView)
{
	//program.use();
	//checkGl();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	int renderCount = 0;
	for (GameObject* object : objects) {
		printf("Rendering %s\n", object->type().c_str());

		// The player doesn't get rendered
		if (object->type() == player->type())
			continue;

		Mesh* mesh = nullptr;

		for (auto& i : meshes) {
			if (i.first == object->type()) {
				mesh = i.second;
				break;
			}
		}


		if (mesh == nullptr) {
			std::cerr << "Could not find mesh for object with mesh name \""
				<< object->meshName() << "\"" << std::endl;
			continue;
		}

		glm::mat4 matTrans = matProjView * object->getModelMatrix();

		//program.setVec3("color", object->color());
		//checkGl();
		//program.setMat4("matTrans", matTrans);
		//checkGl();

		mesh->draw(matTrans, object->color());

		renderCount++;
	}
	if(renderCount == 0) {
		printf("Did not render anything!!!\n");
	}
}

void Application::loadMeshes()
{
	meshes.push_back({ "bullet", new Mesh("lab3r/bullet.obj") });
	meshes.push_back({ "enemy", new Mesh("lab3r/enemy.obj") });
	meshes.push_back({ "powerup", new Mesh("lab3r/powerup.obj") });

	//colliderMesh.load("lab3r/collider.obj");
	levelMesh.load("lab3r/level.obj");
	triMesh.load("lab3r/tri.obj");
}
