#include "RigidBody.h"

const float MIN_LINEAR_THRESHOLD = 0.1f;
const float MIN_ANGULAR_THRESHOLD = 0.1f;

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass) : PhysicsObject(shapeID) {
	m_position = position;
	m_velocity = velocity;
	m_rotation = 0;
	m_angularVelocity = 0;
	m_mass = mass;
}


RigidBody::~RigidBody() {
}

void RigidBody::FixedUpdate(glm::vec2 gravity, float timeStep) {

	if (m_isKinematic) {
		SetVelocity(glm::vec2(0, 0));
	} else {
		m_velocity += gravity * timeStep;
		m_position += m_velocity * timeStep;

		m_velocity -= m_velocity * m_linearDrag * timeStep;
		m_rotation += m_angularVelocity * timeStep;
		m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep;

		if (length(m_velocity) < MIN_LINEAR_THRESHOLD) m_velocity = glm::vec2(0, 0);
		if (abs(m_angularVelocity) < MIN_ANGULAR_THRESHOLD) m_angularVelocity = 0;
	}
}

void RigidBody::ApplyForce(glm::vec2 force, glm::vec2 pos) {
	if (m_isKinematic) return;
	
	m_velocity += force / m_mass;
	m_angularVelocity += (force.y * pos.x - force.x * pos.y) / (m_moment);
}

void RigidBody::SetVelocity(glm::vec2 velocity) {
	m_velocity = velocity;
}
void RigidBody::SetPosition(glm::vec2 position) {
	m_position = position;
}

void RigidBody::ResolveCollision(RigidBody* actor2, glm::vec2 contact, glm::vec2* collisionNormal) {

	// finds the vector between the centres, or uses provided directions of force
	glm::vec2 normal = collisionNormal ? *collisionNormal : glm::normalize(actor2->m_position - m_position);

	// get the vector perpendicular to the collision normal
	glm::vec2 perp(normal.y, -normal.x);

	// determine the total velocity of teh contact points for the two objects, for both linear and rotational
	// 'r' is the radius from axis to application of force
	float r1 = glm::dot(contact - m_position, -perp);
	float r2 = glm::dot(contact - actor2->m_position, perp);

	// velocity of the contacty point on this object
	float v1 = glm::dot(m_velocity, normal) - r1 * m_angularVelocity;
	// velocity of contact point on actor2
	float v2 = glm::dot(actor2->m_velocity, normal) + r2 * actor2->m_angularVelocity;

	if (v1 > v2) { // they're moving closer
		// calculate the effective mass at contact point for each object ie how much the contact point will move due to the force applied
		float mass1 = 1.0f / (1.0f / m_mass + (r1*r1) / m_moment);
		float mass2 = 1.0f / (1.0f / actor2->m_mass + (r2*r2) / actor2->m_moment);

	float elasticity = (m_elasticity + actor2->GetElasticity()) / 2.0f;

	glm::vec2 force = (1.0f + elasticity)*mass1*mass2 / (mass1 + mass2)*(v1 - v2)*normal;

	// apply equal and opposite forces
	ApplyForce(-force, contact - m_position);
	actor2->ApplyForce(force, contact - actor2->m_position);
	}
}
