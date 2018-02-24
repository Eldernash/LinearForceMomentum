#include "Plane.h"
#include "RigidBody.h"


Plane::Plane() : PhysicsObject(PLANE) {
	m_normal = glm::vec2(0, 1);
	m_distance = 0;
}
Plane::Plane(glm::vec2 normal, float distance) : PhysicsObject(PLANE) {
	m_normal = glm::normalize(normal);
	m_distance = distance;
}

Plane::~Plane() {}

void Plane::FixedUpdate(glm::vec2 gravity, float timeStep) {}

void Plane::Draw() {
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = m_normal * m_distance;

	// easy to rotate normal through 90 degrees around Z
	glm::vec2 parallel(m_normal.y, -m_normal.x);
	glm::vec4 colour(1, 1, 1, 1);
	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
	aie::Gizmos::add2DLine(start, end, colour);
}

void Plane::ResetPosition() {}

void Plane::ResolveCollision(RigidBody* actor2, glm::vec2 contact) {

	// the plane isn't moving, so the relative velocity is just actor2's velocity
	glm::vec2 relativeVelocity = actor2->GetVelocity();
	float j = glm::dot(-(1 + actor2->GetElasticity()) * (relativeVelocity), m_normal) / (1 / actor2->GetMass());
	glm::vec2 force = m_normal * j;

	actor2->ApplyForce(force, contact - actor2->GetPosition());
}