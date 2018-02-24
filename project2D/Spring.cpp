#include "Spring.h"
#include "RigidBody.h"

Spring::Spring() : PhysicsObject(BOX) {};
Spring::~Spring() {};

void Spring::FixedUpdate(glm::vec2 gravity, float timeStep) {

	glm::vec2 p1 = m_body1->ToWorld(m_contact2);
	glm::vec2 p2 = m_body2->ToWorld(m_contact1);
	glm::vec2 dist = p2 - p1;
	float length = sqrtf(dist.x * dist.x + dist.y * dist.y);

	// apply dampening
	glm::vec2 relativeVelocity = m_body2->GetVelocity() - m_body1->GetVelocity();

	// F = -kX - bv
	glm::vec2 force = dist * m_springCoefficient * (m_restLength - length) - m_damping * relativeVelocity;

	m_body1->ApplyForce(-force * timeStep, p1 - m_body1->GetPosition());
	m_body2->ApplyForce(-force * timeStep, p2 - m_body2->GetPosition());
}
