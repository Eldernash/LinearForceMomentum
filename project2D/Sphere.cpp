#include "Sphere.h"

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) : RigidBody(SPHERE, position, velocity, 0, mass) {
	m_radius = radius;
	m_colour = colour;
}

Sphere::~Sphere() {}

void Sphere::Draw() {
	aie::Gizmos::add2DCircle(m_position, m_radius, 15, m_colour);
}

bool Sphere::CheckCollision(PhysicsObject* pOther) {
	Sphere* sOther = dynamic_cast<Sphere*>(pOther);
	float radiai = m_radius + sOther->GetRadius();
	float distance = glm::distance(m_position, sOther->GetPosition());
	if (distance <= radiai) {
		return true;
	} else return false;
}