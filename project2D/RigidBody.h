#pragma once
#include "PhysicsObject.h"

class RigidBody
{
public:
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass);
	~RigidBody();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void Debug();
	void ApplyForce(glm::vec2 force);
	void ApplyForceToActor(RigidBody* actor2, glm::vec2 force);

	virtual bool ChechCollision(PhysicsObject* pOther) = 0;

	glm::vec2 GetPosition() { return m_position; }
	float getRotation() { return m_rotation; }
	glm::vec2 GetVelocity() { return m_velocity; }
	float getMass() { return m_mass; }

protected:
	glm::vec2 m_position;
	glm::vec2 m_velocity;
	float m_mass;
	float m_rotation; // 2D so we only need a single float to represent out rotation

};

