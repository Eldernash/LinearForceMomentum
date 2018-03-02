#pragma once
#include "PhysicsObject.h"

class RigidBody : public PhysicsObject
{
public:
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass);
	~RigidBody();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);

	virtual bool CheckCollision(PhysicsObject* pOther) = 0;

	void ApplyForce(glm::vec2 force, glm::vec2 pos);
	void ResolveCollision(RigidBody* actor2, glm::vec2 contact, glm::vec2* collisionNormal = nullptr);
	void Nudge(glm::vec2 dx) { m_position += dx; }

	// sets the attributes of the object
	void SetVelocity(glm::vec2 velocity);
	void SetPosition(glm::vec2 position);
	void SetKinematic(bool kin) { m_isKinematic = kin; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	void SetElasticity(float elasticity) { m_elasticity = elasticity; }

	// gets the attributes of the object
	virtual float GetTotalEnergy(glm::vec2 gravity) { return this->GetLinearEnergy() + this->GetRotationalEnergy() + this->GetGravityEnergy(gravity); }
	float GetMoment() { return m_moment; }
	float GetMass() { return (m_isKinematic) ? (float)INT_MAX : m_mass; }
	float GetElasticity() { return m_elasticity; }
	float GetRotation() { return m_rotation; }
	float GetLinearDrag() { return m_linearDrag; }
	float GetAngularDrag() { return m_angularDrag; }
	float GetAngularVelocity() { return m_angularVelocity; }

	// returns the respective energy values
	float GetLinearEnergy() { return (m_isKinematic) ? 0 : 0.5f * m_mass * glm::dot(m_velocity, m_velocity); }
	float GetRotationalEnergy() { return (m_isKinematic) ? 0 : 0.5f * m_moment * m_angularVelocity * m_angularVelocity; }
	float GetGravityEnergy(glm::vec2 gravity) { return (m_isKinematic) ? 0 : m_mass * glm::dot(m_position, gravity); }

	bool IsKinematic() { return m_isKinematic; }
	glm::vec2 GetPosition() { return m_position; }
	glm::vec2 GetVelocity() { return m_velocity; }

	glm::vec2 ToWorld(glm::vec2 contact) { return m_position + contact; }


protected:

	float m_mass;
	float m_elasticity;
	float m_rotation; // 2D so we only need a single float to represent out rotation
	float m_linearDrag;
	float m_angularDrag;
	float m_angularVelocity;
	float m_moment;

	glm::vec2 m_position;
	glm::vec2 m_velocity;
	glm::vec2 m_localX;
	glm::vec2 m_localY;

	bool m_isKinematic;

};