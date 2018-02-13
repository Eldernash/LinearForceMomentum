#pragma once
#include <Gizmos.h>
#include <glm\glm.hpp>

#include "PhysicsObject.h"

class RigidBody;

class Plane : public PhysicsObject
{
public:
	Plane();
	Plane(glm::vec2 normal, float distance);
	~Plane();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void Debug() {}
	virtual void Draw();
	virtual void ResetPosition();

	void ResolveCollision(RigidBody* actor2, glm::vec2 contact);

	glm::vec2 GetNormal() { return m_normal; }
	float GetDistance() { return m_distance; }

protected:
	glm::vec2 m_normal;
	float m_distance;
};
