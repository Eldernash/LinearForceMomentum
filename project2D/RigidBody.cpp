#include "RigidBody.h"



RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass) {
	m_position = m_position;
	m_velocity = m_velocity;
	m_mass = mass;
	m_rotation = rotation;
}


RigidBody::~RigidBody() {
}

void RigidBody::FixedUpdate(glm::vec2 gravity, float timeStep) {
}

void RigidBody::Debug() {
}

void RigidBody::ApplyForce(glm::vec2 force) {
}

void RigidBody::ApplyForceToActor(RigidBody * actor2, glm::vec2 force) {
}
