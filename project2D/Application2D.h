#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Spring.h"

class Application2D : public aie::Application {
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();
	void CreateSoftBody(glm::vec2 coords);
	void CreateRope(glm::vec2 coord1, glm::vec2 coord2);

	glm::vec2 WorldToGizmo(glm::vec2 coord);

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	PhysicsScene* m_physicsScene;

	glm::vec2 ropeStartPos;

	float m_cameraX, m_cameraY;
	float m_timer;
	float aspectRatio = 16 / 9.f;

	short spawner = 1;

	// booleans for controlling the simulation
	bool running = true;
	bool secondRopeNode = false;
	bool spawnKinematic = false;
};