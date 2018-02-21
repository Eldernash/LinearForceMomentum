#include <glm\ext.hpp>
#include <Gizmos.h>
#include <random>

#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

Application2D::Application2D() {}

Application2D::~Application2D() {}

bool Application2D::startup() {
	srand(time(NULL));

	// increase the 2D line count to maximise the number of objects we can draw
	aie::Gizmos::create(255U,255U,65535U,65535U);

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_physicsScene = new PhysicsScene();
	m_physicsScene->SetGravity(glm::vec2(0, 0));
	m_physicsScene->SetTimeStep(0.01f);

	Sphere* ball1 = new Sphere(glm::vec2(-50, 1), glm::vec2(200, 0), 1, 5, glm::vec4(1, 0, 0, 1));

	Box* box1 = new Box(glm::vec2(50, 0), glm::vec2(10, 10), glm::vec2(0, 0), 2, glm::vec4(0, 0, 1, 1));
	box1->SetKinematic(true);

	int sphereAmount = 50;
	for (int i = 0; i < sphereAmount; i++) {
		Sphere* ball = new Sphere(glm::vec2((rand() % 190) - 100, (rand() % 80) - 50), glm::vec2((rand() % 200) - 10, (rand() % 20) - 10), 1, 3, glm::vec4(1, 0, 0, 1));
		m_physicsScene->AddActor(ball);
	}
	int boxAmount = 50;
	for (int i = 0; i < boxAmount; i++) {
		Box* cube = new Box(glm::vec2((rand() % 190) - 100, (rand() % 80) - 50), glm::vec2(2, 2), glm::vec2((rand() % 200) - 10, (rand() % 20) - 10), 1, glm::vec4(0, 1, 0, 1));
		m_physicsScene->AddActor(cube);
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