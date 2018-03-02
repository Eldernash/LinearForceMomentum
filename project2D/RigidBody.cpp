#include "RigidBody.h"

const float MIN_LINEAR_THRESHOLD = 0.1f;
const float MIN_ANGULAR_THRESHOLD = 0.1f;

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass) : PhysicsObject(shapeID) {
	m_position = position;
	m_velocity = velocity;
	m_rotation = 0;
	m_angularVelocity = 0;
	m_mass = mass;

	float cs = cosf(m_rotation);
	float sn = sinf(m_rotation);
	m_localX = glm::normalize(glm::vec2(cs, sn));
	m_localY = glm::normalize(glm::vec2(-sn, cs));
}


RigidBody::~RigidBody() {}

void RigidBody::FixedUpdate(glm::vec2 gravity, float timeStep) {

	float cs = cosf(m_rotation);
	float sn = sinf(m_rotation);
	m_localX = glm::normalize(glm::vec2(cs, sn));
	m_localY = glm::normalize(glm::vec2(-sn, cs));

	if (m_isKinematic) {
		SetVelocity(glm::vec2(0, 0));
	}
	else {

		float cs = cosf(m_rotation);
		float sn = sinf(m_rotation);
		m_localX = glm::normalize(glm::vec2(cs, sn));
		m_localY = glm::normalize(glm::vec2(-sn, cs));

		m_velocity += gravity * timeStep;
		m_position += m_velocity * timeStep;

		m_velocity -= m_velocity * m_linearDrag * timeStep;
		m_rotation += m_angularVelocity * timeStep;
		m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep;

		if (length(m_velocity) < MIN_LINEAR_THRESHOLD) {
			if (glm::length(m_velocity) < length(gravity) * m_linearDrag * timeStep) {
				m_velocity = glm::vec2(0, 0);
			}
		}
		if (abs(m_angularVelocity) < MIN_ANGULAR_THRESHOLD) {
			m_angularVelocity = 0;
		}
	}
}

void RigidBody::ApplyForce(glm::vec2 force, glm::vec2 pos) {
	m_velocity += force / this->GetMass();
	m_angularVelocity += (force.y * pos.x - force.x * pos.y) / (this->GetMoment());
}

void RigidBody::SetVelocity(glm::vec2 velocity) {
	m_velocity = velocity;
}
void RigidBody::SetPosition(glm::vec2 position) {
	m_position = position;
}

void RigidBody::ResolveCollision(RigidBody * actor2, glm::vec2 contact, glm::vec2* collisionNormal)
{
	glm::vec2 normal = collisionNormal ? *collisionNormal : glm::normalize(actor2->m_position - m_position);

	glm::vec2 perp(normal.y, -normal.x);

	float r1 = glm::dot(contact - m_position, -perp);
	float r2 = glm::dot(contact - actor2->m_position, perp);
	float v1 = glm::dot(m_velocity, normal) - r1 * m_angularVelocity;
	float v2 = glm::dot(actor2->m_velocity, normal) + r2 * actor2->m_angularVelocity;

	if (v1 > v2) {
		float mass1 = 1.0f / (1.0f / this->GetMass() + (r1*r1) / this->GetMoment());
		float mass2 = 1.0f / (1.0f / actor2->GetMass() + (r2*r2) / actor2->GetMoment());

		float elasticity = (m_elasticity = actor2->GetElasticity()) / 2.0f;

		glm::vec2 force = (1.0f + elasticity) * mass1 * mass2 / (mass1 + mass2) * (v1 - v2) * normal;

		ApplyForce(-force, contact - m_position);
		actor2->ApplyForce(force, contact - actor2->m_position);
	}
}
