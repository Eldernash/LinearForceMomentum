#include <glm\ext.hpp>
#include <Gizmos.h>

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
void Application2D::CreateSoftBody(glm::vec2 coords, glm::vec2 dimensions) {

	std::vector<Sphere*> sbList;
	Sphere* sbBall1;
	sbBall1 = new Sphere(coords, glm::vec2(0, 0), 1, 1, glm::vec4(0, 0, 1, 1));
	sbList.push_back(sbBall1);
	for (int x = 1; x < dimensions.x - 1; x++) {
		for (int y = 1; y < dimensions.y - 1; y++) {
			Sphere* sbBall2 = new Sphere(glm::vec2(coords.x + (x * 2), coords.y + (y * 2)), glm::vec2(0, 0), 1, 1, glm::vec4(0, 0, 1, 1));
			sbList.push_back(sbBall2);
		}
	}
	for (auto sbBall : sbList) {

	}
}


bool Application2D::startup() {

	// increase the 2D line count to maximise the number of objects we can draw
	aie::Gizmos::create(255U,255U,65535U,65535U);

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_physicsScene = new PhysicsScene();
	m_physicsScene->SetGravity(glm::vec2(0, -12));
	m_physicsScene->SetTimeStep(0.01f);

	int startX = -40;
	Sphere* ball1;
	Sphere* ball2;
	float ballRadius = 2;
	float mass = 100;

	ball1 = new Sphere(glm::vec2(startX, 0), glm::vec2(0, 0), mass, ballRadius, glm::vec4(1, 0, 0, 1));
	ball1->SetElasticity(0.9f);
	ball1->SetKinematic(true);
	m_physicsScene->AddActor(ball1);

	int numberBalls = 20;
	for (int i = 1; i < numberBalls; i++)
	{
		ball2 = new Sphere(glm::vec2(startX + (i * 5), 0), glm::vec2(0, 0), mass, ballRadius, glm::vec4(1, 0, 0, 1));
		
		m_physicsScene->AddActor(ball2);

		Spring* newSpring = new Spring(ball1, ball2, 2.0f, 500, 5.0f);
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

	if (running) {
		m_physicsScene->Update(deltaTime);
		std::cout << m_physicsScene->GetEnergy() << std::endl;

		if (input->wasKeyPressed(aie::INPUT_KEY_UP)) {
			for (auto iter = springList.begin(); iter != springList.end(); iter++) {
				(*iter)->SetRestLength(1.0f);
			}
		}
		if (input->wasKeyPressed(aie::INPUT_KEY_DOWN)) {
			for (auto iter = springList.begin(); iter != springList.end(); iter++) {
				(*iter)->SetRestLength(2.0f);
			}
		}
	}

	if (input->wasMouseButtonPressed(0)) {
		float mouseX = (float)input->getMouseX();
		float mouseY = (float)input->getMouseY();
		glm::vec2 coords = WorldToGizmo(glm::vec2(mouseX, mouseY));
		if (spawner == 1) {
			Sphere * mSphere = new Sphere(coords, glm::vec2(0, 0), 1, 4, glm::vec4(0, 0, 1, 1));
			m_physicsScene->AddActor(mSphere);
		} else if (spawner == 2) {
			Box * mBox = new Box(coords, glm::vec2(3, 3), glm::vec2(0, 0), 1, glm::vec4(0, 1, 0, 1));
			m_physicsScene->AddActor(mBox);
		}
		else if (spawner == 3) {
			CreateSoftBody(coords, glm::vec2(5, 5));
		}
	}
	if (input->isMouseButtonDown(1)) {
		float mouseX = (float)input->getMouseX();
		float mouseY = (float)input->getMouseY();
		glm::vec2 coords = WorldToGizmo(glm::vec2(mouseX, mouseY));
		m_physicsScene->RemoveObject(coords, 10);
	}

	// changes between the apwned object
	if (input->wasKeyPressed(aie::INPUT_KEY_1)) {
		spawner = 1;
	}
	else if (input->wasKeyPressed(aie::INPUT_KEY_2)) {
		spawner = 2;
	}
	else if (input->wasKeyPressed(aie::INPUT_KEY_3)) {
		spawner = 3;
	}

	// pasuses/resumes the simulation
	if (input->wasKeyPressed(aie::INPUT_KEY_SPACE)) {
		if (running) {
			running = false;
		} else {
			running = true;
		}
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
