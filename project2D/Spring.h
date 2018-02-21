#pragma once

#include "PhysicsObject.h"

class RigidBody;

class Spring : public PhysicsObject
{
public:
	Spring();
	~Spring();

	void FixedUpdate(glm::vec2 gravity, float timeStep);

protected:
	RigidBody* m_body1;
	RigidBody* m_body2;

	glm::vec2 m_contact1;
	glm::vec2 m_contact2;

	float m_damping;
	float m_restLength;
	float m_springCoefficient; // the restoring force
};
