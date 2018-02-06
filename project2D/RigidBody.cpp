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
