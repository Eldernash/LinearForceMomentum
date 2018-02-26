#include "Spring.h"
#include "RigidBody.h"

class RigidBody;

Spring::Spring(RigidBody* body1, RigidBody* body2, float restLength, float springCoefficient, float damping, glm::vec2 contact1, glm::vec2 contact2) : PhysicsObject(SPRING) {
	m_body1 = body1;
	m_body2 = body2;
	m_restLength = restLength;
	m_springCoefficient = springCoefficient;
	m_damping = damping;
	m_contact1 = contact1;
	m_contact2 = contact2;
}

Spring::~Spring() {}

void Spring::FixedUpdate(glm::vec2 gravity, float timeStep) {

	glm::vec2 p2 = m_body2->ToWorld(m_contact2);
	glm::vec2 p1 = m_body1->ToWorld(m_contact1);
	glm::vec2 dist = p2 - p1;
	float length = sqrtf(dist.x*dist.x + dist.y * dist.y);

	float deltaLength = length - m_restLength;
	if (deltaLength > m_restLength) {
		deltaLength = m_restLength;
	}
	float forceMagnitude = deltaLength * m_springCoefficient;

	// apply dampening
	glm::vec2 relativeVelocity = m_body2->GetVelocity() - m_body1->GetVelocity();

	// F = -kx - bv
//	glm::vec2 direction = (m_body1->GetPosition() - m_body2->GetPosition()) / length;
	glm::vec2 force = dist * m_springCoefficient * (m_restLength - length) - m_damping * relativeVelocity;

	m_body1->ApplyForce(-force * timeStep, p1 - m_body1->GetPosition());
	m_body2->ApplyForce(force * timeStep, p2 - m_body2->GetPosition());
}

void Spring::Draw() {
	glm::vec4 colour(1, 1, 0, 1);
	aie::Gizmos::add2DLine(m_body1->GetPosition(), m_body2->GetPosition(), colour);
}