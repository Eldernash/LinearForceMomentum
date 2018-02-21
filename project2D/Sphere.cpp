#include "Sphere.h"

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour) : RigidBody(SPHERE, position, velocity, 0, mass) {
	m_radius = radius;
	m_colour = colour;
	m_elasticity = 0;
	m_isKinematic = false;
	m_linearDrag = 0;
	m_angularDrag = 0;
	m_moment = 0.5f*mass*radius*radius;
}

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, float elasticity) : RigidBody(SPHERE, position, velocity, 0, mass) {
	m_radius = radius;
	m_colour = colour;
	m_elasticity = elasticity;
	m_isKinematic = false;
	m_linearDrag = 0.3f;
	m_angularDrag = 0.1f;
	m_moment = 0.5f*mass*radius*radius;
}

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, float elasticity, bool kinematic) : RigidBody(SPHERE, position, velocity, 0, mass) {
	m_radius = radius;
	m_colour = colour;
	m_elasticity = elasticity;
	m_isKinematic = kinematic;
	m_linearDrag = 0.3f;
	m_angularDrag = 0.1f;
	m_moment = 0.5f*mass*radius*radius;
}

Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, float elasticity, bool kinematic, float lDrag, float aDrag) : RigidBody(SPHERE, position, velocity, 0, mass) {
	m_radius = radius;
	m_colour = colour;
	m_elasticity = elasticity;
	m_isKinematic = kinematic;
	m_linearDrag = lDrag;;
	m_angularDrag = aDrag;
	m_moment = 0.5f*mass*radius*radius;
}

Sphere::~Sphere() {}

void Sphere::Draw() {
	glm::vec2 end = glm::vec2(std::cos(m_rotation), std::sin(m_rotation)) * m_radius;

	aie::Gizmos::add2DCircle(m_position, m_radius, 15, m_colour);
	aie::Gizmos::add2DLine(m_position, m_position + end, glm::vec4(1, 1, 1, 1));
}

void Sphere::RicochetPerp() {
	m_velocity = -m_velocity;
}

bool Sphere::CheckCollision(PhysicsObject* pOther) {
	return false;
}