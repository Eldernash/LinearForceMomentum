#include "Box.h"



Box::Box(glm::vec2 position, glm::vec2 extents, glm::vec2 velocity, float mass, glm::vec4 colour) : RigidBody(BOX, position, velocity, 0, mass) {
	m_extents = extents;
	m_colour = colour;
	m_elasticity = 1;
	isKinematic = false;
	m_linearDrag = 0.3f;
	m_angularDrag = 0.1f;
	m_moment =  1.0f / 12.0f * mass * m_extents.x * m_extents.y;
}

Box::~Box(){}

void Box::Draw() {
	// if only using rotation
	// glm::vec4 transform = glm::rotate(m_rotation, glm::vec3(0,0,1));
	// aie::Giamos::add2DAABBFilled(GetCenter(), m_extents, m_colour, &transform);

	//draw using local axes
	glm::vec2 p1 = m_position - m_localX * m_extents.x - m_localY * m_extents.y;
	glm::vec2 p2 = m_position + m_localX * m_extents.x - m_localY * m_extents.y;
	glm::vec2 p3 = m_position - m_localX * m_extents.x + m_localY * m_extents.y;
	glm::vec2 p4 = m_position + m_localX * m_extents.x + m_localY * m_extents.y;

	aie::Gizmos::add2DTri(p1, p2, p4, m_colour);
	aie::Gizmos::add2DTri(p1, p4, p3, m_colour);
}

void Box::FixedUpdate(glm::vec2 gravity, float timeStep) {
	RigidBody::FixedUpdate(gravity, timeStep);

	//store the local axes
	float cs = cosf(m_rotation);
	float sn = sinf(m_rotation);
	m_localX = glm::normalize(glm::vec2(cs, sn));
	m_localY = glm::normalize(glm::vec2(-sn, cs));
}

bool Box::CheckCollision(PhysicsObject* pOther) {
	return false;
}