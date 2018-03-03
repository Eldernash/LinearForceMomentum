#include "Box.h"



Box::Box(glm::vec2 position, glm::vec2 extents, glm::vec2 velocity, float mass, glm::vec4 colour) : RigidBody(BOX, position, velocity, 0, mass) {
	m_extents = extents;
	m_colour = colour;
	m_elasticity = 1;
	m_isKinematic = false;
	m_linearDrag = 0;
	m_angularDrag = 0;
	m_moment =  1.0f / 12.0f * mass * m_extents.x * 2.0f * m_extents.y * 2.0f;

}

Box::Box(glm::vec2 position, glm::vec2 extents, glm::vec2 velocity, float mass, bool kinematic, glm::vec4 colour) : RigidBody(BOX, position, velocity, 0, mass) {
	m_extents = extents;
	m_colour = colour;
	m_elasticity = 1;
	m_isKinematic = false;
	m_linearDrag = 0;
	m_angularDrag = 0;
	m_isKinematic = kinematic;
	m_moment = 1.0f / 12.0f * mass * m_extents.x * 2.0f * m_extents.y * 2.0f;
}

Box::~Box(){}

void Box::FixedUpdate(glm::vec2 gravity, float timeStep) {
		RigidBody::FixedUpdate(gravity, timeStep);
}
void Box::Draw() {
	//draw using local axes
	glm::vec2 p1 = m_position - m_localX * m_extents.x - m_localY * m_extents.y;
	glm::vec2 p2 = m_position + m_localX * m_extents.x - m_localY * m_extents.y;
	glm::vec2 p3 = m_position - m_localX * m_extents.x + m_localY * m_extents.y;
	glm::vec2 p4 = m_position + m_localX * m_extents.x + m_localY * m_extents.y;

	aie::Gizmos::add2DTri(p1, p2, p4, m_colour);
	aie::Gizmos::add2DTri(p1, p4, p3, m_colour);
}

bool Box::CheckCollision(PhysicsObject* pOther) {
	return false;
}

bool Box::CheckBoxCorners(Box& box, glm::vec2& contact, int& numContacts, float &pen, glm::vec2& edgeNormal) {
	float minX, maxX, minY, maxY;
	float boxW = box.GetExtents().x * 2;
	float boxH = box.GetExtents().y * 2;
	int numLocalContacts = 0;
	glm::vec2 localContact(0, 0);

	bool first = true;
	// for the four corners of the box
	for (float x = -box.GetExtents().x; x < boxW; x += boxW) {
		for (float y = -box.GetExtents().y; y < boxH; y += boxH) {

			// position in worldspace
			glm::vec2 p = box.GetPosition() + x * box.GetLocalX() + y * box.GetLocalY();

			// position in our box's space
			glm::vec2 p0(glm::dot(p - m_position, m_localX), glm::dot(p - m_position, m_localY));

			if (first || p0.x < minX) minX = p0.x;
			if (first || p0.x > maxX) maxX = p0.x;
			if (first || p0.y < minY) minY = p0.y;
			if (first || p0.y > maxY) maxY = p0.y;

			if (p0.x >= -m_extents.x && p0.x <= m_extents.x && p0.y >= -m_extents.y && p0.y <= m_extents.y) {
				numLocalContacts++;
				localContact += p0;
			}
			first = false;
		}
	}

	if (maxX <= -m_extents.x || minX >= m_extents.x || maxY <= -m_extents.y || minY >= m_extents.y) return false;
	if (numLocalContacts == 0) return false;
	bool res = false;

	contact += m_position + (localContact.x*m_localX + localContact.y*m_localY) / (float)numLocalContacts;
	numContacts++;

	float pen0 = m_extents.x - minX;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = m_localX;
		pen = pen0;
		res = true;
	}
	pen0 = maxX + m_extents.x;

	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = -m_localX;
		pen = pen0;
		res = true;
	}
	pen0 = m_extents.y - minY;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = m_localY;
		pen = pen0;
		res = true;
	}
	pen0 = maxY + m_extents.y;
	if (pen0 > 0 && (pen0 < pen || pen == 0)) {
		edgeNormal = -m_localY;
		pen = pen0;
		res = true;
	}

	return res;
}
