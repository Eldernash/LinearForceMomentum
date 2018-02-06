#pragma once

#include <Gizmos.h>
#include <glm\geometric.hpp>

#include "RigidBody.h"

class Sphere : public RigidBody
{
public:
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour);
	~Sphere();

	virtual void Draw();
	virtual bool CheckCollision(PhysicsObject* pOther);

	float GetRadius() { return m_radius; }
	glm::vec4 GetColour() { return m_colour; }

protected:
	float m_radius;
	glm::vec4 m_colour;

};