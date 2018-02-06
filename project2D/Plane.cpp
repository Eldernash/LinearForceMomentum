#include "Plane.h"



Plane::Plane() : PhysicsObject(ShapeType::PLANE) {
	m_normal = glm::vec2(0, 1);
	m_distance = 0;
}
Plane::Plane(glm::vec2 normal, float distance) : PhysicsObject(ShapeType::PLANE) {
	m_normal = normal;
	m_distance = distance;
}

Plane::~Plane() {}

void Plane::Draw() {
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = m_normal * m_distance;
	// easy to rotate normal through 90 degrees around Z
	glm::vec2 parallel(m_normal.y, -m_normal.x);
	glm::vec4 colour(1, 1, 1, 1);
	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
	aie::Gizmos::add2DLine(start, end, colour);
}