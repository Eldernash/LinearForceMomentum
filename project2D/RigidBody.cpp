#include "RigidBody.h"



RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass) : PhysicsObject(shapeID) {
	m_position = position;
	m_velocity = velocity;
	m_mass = mass;
	m_rotation = rotation;
}


RigidBody::~RigidBody() {
}

void RigidBody::FixedUpdate(glm::vec2 gravity, float timeStep) {
	ApplyForce(gravity * m_mass * timeStep);
	m_position += m_velocity * timeStep;
}

void RigidBody::Debug() {
}

void RigidBody::ApplyForce(glm::vec2 force) {
	m_velocity += force / m_mass;
}

void RigidBody::ApplyForceToActor(RigidBody * actor2, glm::vec2 force) {
	actor2->ApplyForce(force);
	this->ApplyForce(-force);
}

void RigidBody::SetVelocity(glm::vec2 velocity) {
	m_velocity = velocity;
}

void RigidBody::ResolveCollision(RigidBody* actor2) {

	glm::vec2 normal = glm::normalize(actor2->GetPosition() - m_position);
	glm::vec2 relativeVelocity = actor2->GetVelocity() - m_velocity;

	// if objects are movingapart, don't
	if (glm::dot(normal, relativeVelocity) > 0) {
		return;
	}

	float elasticity = 1;
	float j = glm::dot(-(1 + elasticity) * (relativeVelocity), normal) / glm::dot(normal, normal * ((1 / m_mass) + (1 / actor2->getMass())));

	glm::vec2 force = normal * j;

	ApplyForceToActor(actor2, force);

}