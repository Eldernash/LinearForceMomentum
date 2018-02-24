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
	void SetVelocity(glm::vec2 velocity);
	void SetPosition(glm::vec2 position);
	void SetKinematic(bool kin) { m_isKinematic = kin; }
	void SetElasticity(float elasticity) { m_elasticity = elasticity; }
	void Nudge(glm::vec2 dx) { m_position += dx; }

	float GetMoment() { return m_moment; }
	float GetMass() { if (m_isKinematic) return FLT_MAX; else return m_mass; }
	float GetElasticity() { return m_elasticity; }
	float GetRotation() { return m_rotation; }
	float GetLinearDrag() { return m_linearDrag; }
	float GetAngularDrag() { return m_angularDrag; }
	float GetAngularVelocity() { return m_angularVelocity; }

	// returns the relevant energy
	float GetLinearEnergy() { return 0.5f*m_mass*glm::dot(m_velocity, m_velocity); }
	float GetRotationalEnergy() { return 0.5f * m_moment * m_angularVelocity * m_angularVelocity; }
	float GetGravityEnergy(glm::vec2 gravity) { return -m_mass * glm::dot(m_position, gravity); }

	virtual float GetTotalEnergy(glm::vec2 gravity) { return this->GetLinearEnergy() + this->GetRotationalEnergy() + this->GetGravityEnergy(gravity); }

	bool IsKinematic() { return m_isKinematic; }

	glm::vec2 GetPosition() { return m_position; }
	glm::vec2 GetVelocity() { return m_velocity; }
	glm::vec2 ToWorld(glm::vec2 contact);

protected:
	float m_mass;
	float m_elasticity;
	float m_rotation; // 2D so we only need a single float to represent out rotation
	float m_linearDrag;
	float m_angularDrag;
	float m_angularVelocity;
	float m_moment;

	bool m_isKinematic;

	glm::vec2 m_position;
	glm::vec2 m_velocity;

	// store the local x,y axes if the box based on its anfle of rotation
	glm::vec2 m_localX;
	glm::vec2 m_localY;

};