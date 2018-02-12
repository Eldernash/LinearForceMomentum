#include <glm\ext.hpp>

#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include <Gizmos.h>

Application2D::Application2D() {}

Application2D::~Application2D() {}

bool Application2D::startup() {
	
	// increase the 2D line count to maximise the number of objects we can draw
	aie::Gizmos::create(255U,255U,65535U,65535U);

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_physicsScene = new PhysicsScene();
	m_physicsScene->SetGravity(glm::vec2(0, 0));
	m_physicsScene->SetTimeStep(0.01f);

	int ballAmount = 150;
	for (int i = 0; i < ballAmount; i++) {
		Sphere* sph = new Sphere(glm::vec2((rand() % 200) - 100, (rand() % 100) - 50), glm::vec2((rand() % 100) - 500, (rand() % 100) - 50), 3, 2, glm::vec4(0.1, 0.1, 0.1, 1));
		m_physicsScene->AddActor(sph);
	}
	Sphere* ball1 = new Sphere(glm::vec2(-10, 0), glm::vec2(0, 0), 1000, 5, glm::vec4(1, 1, 0, 1));

	m_physicsScene->AddActor(ball1);

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

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application2D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	m_2dRenderer->setCameraPos(m_cameraX, m_cameraY);

	// begin drawing sprites
	m_2dRenderer->begin();

	m_physicsScene->UpdateGizmo();
	static float aspectRatio = 16 / 9.f;
	aie::Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));
	

	// output some text, uses the last used colour
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 0, 720 - 32);
	m_2dRenderer->drawText(m_font, "Press ESC to quit!", 0, 720 - 64);

	// done drawing sprites
	m_2dRenderer->end();
}