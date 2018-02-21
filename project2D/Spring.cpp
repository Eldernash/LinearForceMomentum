#include "Spring.h"
#include "RigidBody.h"

Spring::Spring() : PhysicsObject(BOX) {};
Spring::~Spring() {};

void Spring::FixedUpdate(glm::vec2 gravity, float timeStep) {

	//glm::vec2 p1 = m_body1->ToWorld(m_contact1);
	//glm::vec2 p2 = m_body2->ToWorld(m_contact2);
}