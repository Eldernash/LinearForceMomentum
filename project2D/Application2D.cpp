#include <glm\ext.hpp>
#include <Gizmos.h>
#include <random>

#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

Application2D::Application2D() {}

Application2D::~Application2D() {}

glm::vec2 Application2D::WorldToGizmo(glm::vec2 coord) {
	float scaleFactor;
	scaleFactor = 200.0f / 1280.0f;
	glm::vec2 finalCoord = (coord - glm::vec2(getWindowWidth() / 2, getWindowHeight() / 2)) * scaleFactor;
	return finalCoord;
}

bool Application2D::startup() {
	srand(time(NULL));

	// increase the 2D line count to maximise the number of objects we can draw
	aie::Gizmos::create(255U,255U,65535U,65535U);

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_physicsScene = new PhysicsScene();
	m_physicsScene->SetGravity(glm::vec2(0, -10));
	m_physicsScene->SetTimeStep(0.01f);

	int startX = -70;
	Sphere* ball1;
	Sphere* ball2;
	float ballRadius = 0.5f;
	float mass = 1;

	Sphere* ball = new Sphere(glm::vec2(-40, 20), glm::vec2(0, -20), 6, 2, glm::vec4(0, 1, 0, 1));
	m_physicsScene->AddActor(ball);

	ball1 = new Sphere(glm::vec2(startX, 0), glm::vec2(0, 0), mass, ballRadius, glm::vec4(1, 0, 0, 1));
	ball1->SetElasticity(0.9f);
	ball1->SetKinematic(true);
	m_physicsScene->AddActor(ball1);
	int numberBalls = 50;
	for (int i = 1; i < numberBalls; i++)
	{
		ball2 = new Sphere(glm::vec2(startX + i, 0), glm::vec2(0, 0), mass, ballRadius, glm::vec4(1, 0, 0, 1));
		if (i == numberBalls - 1) {
			ball2->SetKinematic(true);
		}
		m_physicsScene->AddActor(ball2);

		Spring* newSpring = new Spring(ball1, ball2, 2.0f, 500, 50.0f);
		springList.push_back(newSpring);

		m_physicsScene->AddActor(newSpring);
		ball1 = ball2;
	}
	Plane* line1 = new Plane(glm::vec2(1, 0), 100);
	Plane* line2 = new Plane(glm::vec2(1, 0), -100);
	Plane* line3 = new Plane(glm::vec2(0, 1), 50);
	Plane* line4 = new Plane(glm::vec2(0, 1), -50);

	m_physicsScene->AddActor(line1);
	m_physicsScene->AddActor(line2);
	m_physicsScene->AddActor(line3);
	m_physicsScene->AddActor(line4);

	m_cameraX = 0;
	m_cameraY = 0;
	m_timer = 0;

	return true;
}

void Application2D::shutdown() {
	
	delete m_font;
	delete m_2dRenderer;
}

void Application2D::update(float deltaTime) {

	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

	aie::Gizmos::clear();

	m_physicsScene->Update(deltaTime);

	std::cout << m_physicsScene->GetEnergy() << std::endl;

	if (input->isKeyDown(aie::INPUT_KEY_UP)) {
		for (auto iter = springList.begin(); iter != springList.end(); iter++) {
			(*iter)->SetRestLength(1.0f);
		}
	}
	if (input->isKeyDown(aie::INPUT_KEY_DOWN)) {
		for (auto iter = springList.begin(); iter != springList.end(); iter++) {
			(*iter)->SetRestLength(2.0f);
		}
	}

	if (input->wasMouseButtonPressed(0)) {
		float mouseX = input->getMouseX();
		float mouseY = input->getMouseY();
		glm::vec2 coords = WorldToGizmo(glm::vec2(mouseX, mouseY));
		Sphere* mBall = new Sphere(coords, glm::vec2(0, 0), 1, 2, glm::vec4(0, 0, 1, 1));
		//Box* mBox = new Box*(coords, glm::vec2(80,0)
		m_physicsScene->AddActor(mBall);
	}

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE)) {
		quit();
	}
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	m_2dRenderer->setCameraPos(m_cameraX, m_cameraY);

	// begin drawing sprites
	m_2dRenderer->begin();

	m_physicsScene->UpdateGizmo();
	
	aie::Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));
	
	// output some text, uses the last used colour
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 720 - 64);

	// done drawing sprites
	m_2dRenderer->end();
}
