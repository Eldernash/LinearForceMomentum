#pragma once
#include "PhysicsObject.h"
#include <Gizmos.h>

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

	glm::vec2 GetNormal() { return m_normal; }
	float GetDistance() { return m_distance; }

protected:
	glm::vec2 m_normal;
	float m_distance;
};
