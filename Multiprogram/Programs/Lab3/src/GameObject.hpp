#ifndef GAME_OBJECT_HPP_INCLUDED
#define GAME_OBJECT_HPP_INCLUDED

#include <string>
#include "Misc.hpp"

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include <vector>

const float ROOM_WIDTH = 40.0f;
const float ROOM_HEIGHT = 40.0f;

class GameObject
{
public:
	virtual void update(float deltaTime, InputState inputState) { }
	virtual void onCollision(GameObject& other) { }

	virtual glm::mat4 getModelMatrix();

	// These functions must be implemented by child classes. The radius function
	// is used for collision detection and the type 
	virtual float		radius()	= 0;
	virtual std::string type()		= 0;
	virtual std::string meshName()	= 0;
	virtual glm::vec3	color() { return glm::vec3{ 1, 1, 1 }; }

	glm::vec3	pos = { 0, 0, 0 };

	void markForDeletion() {
		_shouldDelete = true;
	}
	void spawnObject(GameObject* object) {
		_toSpawn = object;
	}

	bool shouldDelete() { return _shouldDelete; }

	GameObject* getObjectToSpawn() {
		GameObject* temp = _toSpawn;
		_toSpawn = nullptr;
		return temp;
	}

private:
	bool _shouldDelete = false;
	GameObject* _toSpawn = nullptr;
};

class Player : public GameObject
{
public:
	void update(float deltaTime, InputState inputState);
	void onCollision(GameObject& other);

	glm::mat4 getViewMatrix();

	float		radius()	{ return 0.1f; }
	std::string	type()		{ return "player"; }
	std::string meshName()	{ return "player"; }
	
	float getHeading() { return heading; }

	void decrementBulletCount() { bulletCount--;  }

protected:
	float		heading = 0.0f;
	float		moveSpeed = 3.0;
	float		shootTimer = 0.0f;
	int			bulletCount = 0;

	const float TURN_SPEED = 3.1415;
	const float TIME_BETWEEN_SHOTS = 0.2f;
	const int	MAX_BULLETS = 3;
};

class Bullet : public GameObject
{
public:
	Bullet(Player& parent);
	void update(float deltaTime, InputState inputState);
	void onCollision(GameObject& other);

	glm::mat4 getModelMatrix();

	float radius()			{ return 0.1f; }
	glm::vec3 color()		{ return glm::vec3{ 0, 0, 1 }; }
	std::string type()		{ return "bullet"; }
	std::string meshName()	{ return "bullet"; }

protected:
	Player& _parent;

	const float SPEED = 8.0f;

	glm::vec3	_velocity;
	glm::vec3	_startPos;
};

class Powerup : public GameObject
{
public:
	Powerup() {}
	Powerup(float x, float z);

	glm::mat4 getModelMatrix();

	void onCollision(GameObject& other);
	void update(float deltaTime, InputState inputState);

	float radius()			{ return 0.25f; }
	glm::vec3 color()		{ return glm::vec3{ 0, 1, 0 }; }
	std::string type()		{ return "powerup"; }
	std::string meshName()	{ return "powerup"; }

private:
	float time;
};

class Enemy : public GameObject
{
public:
	Enemy(float x, float z);

	glm::mat4 getModelMatrix();
	void update(float deltaTime, InputState inputState);
	void onCollision(GameObject& other);

	float radius() { return 0.7f; }
	glm::vec3 color() { return glm::vec3{ 1, 0, 0 }; }
	std::string type() { return "enemy"; }
	std::string meshName() { return "enemy"; }

protected:
	float		speed;
	float		heading;
	float		timeUntilChange;
	glm::mat4	matRotate;
};


#endif//GAME_OBJECT_HPP_INCLUDED