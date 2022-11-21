#include "GameObject.hpp"

#include <iostream>

#include "Misc.hpp"

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>


glm::mat4 GameObject::getModelMatrix()
{
	return glm::translate(pos);
}

//=============================================================================
//							PLAYER IMPLEMENTATION
//=============================================================================

void Player::update(float deltaTime, InputState inputState)
{
	float moveAmount = moveSpeed * deltaTime;
	float turnAmount = TURN_SPEED * deltaTime;

	if (inputState.shoot && shootTimer <= 0.0f && bulletCount < MAX_BULLETS) {
		spawnObject(new Bullet(*this));
		shootTimer = TIME_BETWEEN_SHOTS;
		bulletCount++;
	}

	glm::vec3 newPos = pos;

	shootTimer -= deltaTime;

	if (inputState.turnLeft)
		heading -= turnAmount;
	if (inputState.turnRight)
		heading += turnAmount;

	double cos = glm::cos(heading);
	double sin = glm::sin(heading);

	if (inputState.moveForward) {
		newPos.x += moveAmount * cos;
		newPos.z += moveAmount * sin;
	}
	if (inputState.moveBackward) {
		newPos.x -= moveAmount * cos;
		newPos.z -= moveAmount * sin;
	}
	if (inputState.moveLeft) {
		newPos.x += moveAmount * sin;
		newPos.z -= moveAmount * cos;
	}
	if (inputState.moveRight) {
		newPos.x -= moveAmount * sin;
		newPos.z += moveAmount * cos;
	}

	if (abs(newPos.x) < ROOM_WIDTH / 2.0f - radius() 
		&& abs(newPos.z) < ROOM_HEIGHT / 2.0f - radius()) {
		pos = newPos;
	}
}

void Player::onCollision(GameObject& other)
{
	////printf("PLAYER COLLIDED++++++++++++++++++++++++++!\n");

	if (other.type() == "powerup")
		moveSpeed += 1.0f;
	else if (other.type() == "enemy")
		moveSpeed -= 1.0f;

	moveSpeed = glm::clamp(moveSpeed, 0.5f, 10.0f);
}

glm::mat4 Player::getViewMatrix()
{
	glm::vec3 lookVector = glm::vec3{
		glm::cos(heading),
		0.0f,
		glm::sin(heading)
	};
	glm::vec3 poi = pos + lookVector;
	return glm::lookAt(pos, poi, glm::vec3{ 0, 1, 0 });
}

//=============================================================================
//							BULLET IMPLEMENTATION
//=============================================================================

Bullet::Bullet(Player& parent) : _parent(parent)
{
	glm::vec3 direction{
		glm::cos(parent.getHeading()),
		0.0f,
		glm::sin(parent.getHeading())
	};

	_startPos = parent.pos + 0.1f * direction;
	pos = _startPos;
	_velocity = SPEED * direction;
	
	////printf("BULLET SHOT++++++++++++++++++++++++++!\n");
}

void Bullet::update(float deltaTime, InputState inputState) {
	pos += deltaTime * _velocity;

	// The bullet should never leave the bounds of the room. Since the room is
	// symetrical on each axis, we can check if the absolute value of the coords
	// are within the positive region of the room, and if not, this bullet is
	// deleted
	if (abs(pos.x) >= ROOM_WIDTH / 2.0f || abs(pos.z) >= ROOM_HEIGHT / 2.0f) {
		////printf("BULLET GONE++++++++++++++++++++++++++!\n");
		markForDeletion();
		_parent.decrementBulletCount();
	}
}

void Bullet::onCollision(GameObject& other) {
	////printf("BULLET COLLIDED++++++++++++++++++++++++++!\n");

	if (other.type() != "player") {
		markForDeletion();
		_parent.decrementBulletCount();
	}
}

glm::mat4 Bullet::getModelMatrix() {
	const float scale = 0.1f;

	return glm::translate(pos) * glm::scale(glm::vec3 { scale, scale, scale });
}

//=============================================================================
//							POWERUP IMPLEMENTATION
//=============================================================================

Powerup::Powerup(float x, float z)
{
	pos = glm::vec3(x, 0.0f, z);
}

void Powerup::update(float deltaTime, InputState inputState)
{
	time += deltaTime;
}

void Powerup::onCollision(GameObject& other)
{
	//printf("POWERUP COLLIDED++++++++++++++++++++++++++!\n");
	if (other.type() == "player" || other.type() == "bullet")
		markForDeletion();
}

glm::mat4 Powerup::getModelMatrix()
{
	const float scale = 0.5f;

	glm::mat4 matModel(1.0f);

	// This is the axis we want the top of the powerup to face
	glm::vec3 axis = glm::normalize(glm::vec3{
		glm::cos(time) / 5.0f,
		1.0f,
		glm::sin(time) / 5.0f
		});

	matModel = glm::translate(matModel, pos);
	matModel = glm::rotate(matModel, glm::radians(90.0f), glm::vec3{ 1,0,0 });
	matModel *= glm::lookAt(glm::vec3{0,0,0}, axis, glm::vec3{1.0f, 0.0f, 0.0f});
	matModel = glm::scale(matModel, glm::vec3(scale, scale, scale));

	return matModel;
}

//=============================================================================
//							ENEMY IMPLEMENTATION
//=============================================================================

Enemy::Enemy(float x, float z)
{
	pos = glm::vec3{ x, 0, z };

	timeUntilChange = 0.0f;
	matRotate = glm::mat4(1.0f);
}

void Enemy::update(float deltaTime, InputState input)
{
	if (timeUntilChange <= 0.0f) {
		timeUntilChange = glm::linearRand(3.0f, 5.0f);
		speed = glm::linearRand(1.0f, 6.0f);
		heading = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
	}

	glm::vec3 direction = glm::vec3{ glm::cos(heading), 0.0f, glm::sin(heading) };
	glm::vec3 newPos = pos + deltaTime * speed * direction;

	timeUntilChange -= deltaTime;

	if (newPos.x <= -(ROOM_WIDTH / 2.0f - radius()))
		heading = glm::radians(0.0f);
	else if (newPos.x >= (ROOM_WIDTH / 2.0f - radius()))
		heading = glm::radians(180.0f);
	else if (newPos.z <= -(ROOM_HEIGHT / 2.0f - radius()))
		heading = glm::radians(90.0f);
	else if (newPos.z >= (ROOM_HEIGHT / 2.0f - radius()))
		heading = glm::radians(270.0f);

	direction = glm::vec3{ glm::cos(heading), 0.0f, glm::sin(heading) };
	pos += deltaTime * speed * direction;

	// Rotate the model so it looks like the spikey boy is rolling
	glm::vec3 rotationAxis = -glm::cross(direction, glm::vec3{0.0f, 1.0f, 0.0f});
	glm::mat4 nextRotation = glm::rotate(deltaTime * speed, rotationAxis);
	matRotate = nextRotation * matRotate;
}

void Enemy::onCollision(GameObject& other)
{
	//printf("ENEMY COLLIDED++++++++++++++++++++++++++!\n");

	if (other.type() == "player" || other.type() == "bullet")
		markForDeletion();
}

glm::mat4 Enemy::getModelMatrix()
{
	float scale = 1.0f;

	glm::mat4 matModel(1.0f);
	matModel = glm::translate(matModel, pos);
	matModel *= matRotate;
	matModel = glm::scale(matModel, { scale, scale, scale });
	return matModel;
}