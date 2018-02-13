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

	float getMass() { if (isKinematic) return INT_MAX; else return m_mass; }
	float GetElasticity() { return m_elasticity; }
	float getRotation() { return m_rotation; }
	float GetLinearDrag() { return m_linearDrag; }
	float GetAngularDrag() { return m_angularDrag; }

	glm::vec2 GetPosition() { return m_position; }
	glm::vec2 GetVelocity() { return m_velocity; }

protected:
	glm::vec2 m_position;
	glm::vec2 m_velocity;

	float m_mass;
	float m_elasticity;
	float m_rotation; // 2D so we only need a single float to represent out rotation
	float m_linearDrag;
	float m_angularDrag;
	float m_angularVelocity;
	float m_moment;

	bool isKinematic;

};