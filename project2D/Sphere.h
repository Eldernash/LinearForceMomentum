#pragma once

#include <Gizmos.h>
#include <glm\geometric.hpp>

#include "RigidBody.h"

class Sphere : public RigidBody
{
public:
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour);
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, float elasticity);
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, float elasticity, bool kinematic);
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, float elasticity, bool kinematic, float lDrag, float aDrag);
	~Sphere();

	virtual void Draw();
	virtual bool CheckCollision(PhysicsObject* pOther);

	void RicochetPerp();

	float GetRadius() { return m_radius; }
	glm::vec4 GetColour() { return m_colour; }

protected:
	float m_radius;
	glm::vec4 m_colour;

};