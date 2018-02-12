#include "Sphere.h"

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) : RigidBody(SPHERE, position, velocity, 0, mass) {
	m_radius = radius;
	m_colour = colour;
}

Sphere::~Sphere() {}

void Sphere::Draw() {
	aie::Gizmos::add2DCircle(m_position, m_radius, 15, m_colour);
}

void Sphere::RicochetPerp() {
	m_velocity = -m_velocity;
}

bool Sphere::CheckCollision(PhysicsObject* pOther) {
	return false;
}