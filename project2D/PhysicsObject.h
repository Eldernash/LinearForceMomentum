#pragma once

#include <glm\glm.hpp>

enum ShapeType {
	JOINT = -1,
	PLANE = 0,
	SPHERE,
	BOX,
	SHAPE_COUNT
};
	//SPRING,

class PhysicsObject
{
public:
	PhysicsObject(ShapeType a_shapeID) : m_shapeID(a_shapeID) {}
	~PhysicsObject() {}

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep) = 0;
	virtual void Draw() = 0;
	virtual void ResetPosition() {};
	virtual float GetTotalEnergy(glm::vec2 gravity) { return 0; }
	ShapeType GetShapeID() { return m_shapeID; }



protected:
	ShapeType m_shapeID;
};