#pragma once
#include <Gizmos.h>
#include "RigidBody.h"

class Box : public RigidBody
{
public:
	Box(glm::vec2 position, glm::vec2 ext, glm::vec2 velocity, float mass, glm::vec4 colour);
	Box(glm::vec2 position, glm::vec2 ext, glm::vec2 velocity, float mass, bool kinematic, glm::vec4 colour);
	~Box();

	void Draw();
	void FixedUpdate(glm::vec2 gravity, float timeStep);

	virtual bool CheckCollision(PhysicsObject* pOther);

	float GetWidth() { return m_extents.x * 2; }
	float GetHeight() { return m_extents.y * 2; }
	float GetMoment() { return m_moment; }
	glm::vec2 GetExtents() { return m_extents; }
	glm::vec2 GetLocalX() { return m_localX; }
	glm::vec2 GetLocalY() { return m_localY; }

	bool CheckBoxCorners(Box& box, glm::vec2& contact, int& numContacts, float& pen, glm::vec2& edgeNormal);

protected:

	glm::vec2 m_extents; // the halfedge lengths
	glm::vec4 m_colour;

	// store the local x,y axes if the box based on its anfle of rotation

};

