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

void Application2D::CreateRope(glm::vec2 coord1, glm::vec2 coord2) {
	glm::vec2 p1 = coord1;
	glm::vec2 p2 = coord2;
	float density = 5.0f;

	glm::vec2 dist = p1 - p2;
	glm::vec2 direction = glm::normalize(dist);

	Sphere * rSphere1 = new Sphere(glm::vec2(p2 + (direction * (0 * density))), glm::vec2(0, 0), 1, 1, glm::vec4(1, 1, 1, 1));
	m_physicsScene->AddActor(rSphere1);
	rSphere1->SetKinematic(true);

	for (int i = 1; i < glm::length(dist) / density; i++) { // creates spheres and springs to form the rope
		Sphere * rSphere2 = new Sphere(glm::vec2(p2 + (direction * (i * density))), glm::vec2(0, 0), 1, 1, glm::vec4(1, 1, 1, 1));
		m_physicsScene->AddActor(rSphere2);

		Spring * link = new Spring(rSphere1, rSphere2, density / 2, 50, 0.1f);
		m_physicsScene->AddActor(link);
		rSphere1 = rSphere2;
	}

	rSphere1->SetKinematic(true);
}

void Application2D::CreateSoftBody(glm::vec2 coords) {

	int dimX = 5;
	int dimY = 5;

	float radius = 1;

	bool first = true;

	std::vector<Sphere*> sbList;

	Sphere* sbBall1;
	sbBall1 = new Sphere(coords, glm::vec2(0, 0), 1, radius, glm::vec4(0, 0, 1, 1));
	sbList.push_back(sbBall1);
	m_physicsScene->AddActor(sbBall1);

	// creates spheres in the determined size
	for (int x = 0; x < dimX; x++) {
		for (int y = 0; y < dimY; y++) {
			if (!first) {
				Sphere* sbBall2 = new Sphere(glm::vec2(coords.x + (x *  radius * 2), coords.y + (y * radius * 2)), glm::vec2(0, 0), 1, radius, glm::vec4(0, 0, 1, 1));
				sbList.push_back(sbBall2);
				m_physicsScene->AddActor(sbBall2);
			} else {
				first = false;
			}
		}
	}

	// links the spheres together
	for (int x = 0; x < dimX; x++) {
		for (int y = 0; y < dimY; y++) {
			int position = (x * dimX) + y;
			if (position + 1 < dimX * dimY && position + 1 != (x+1) * dimX) { // links along the y axis
				Spring* spring1 = new Spring(sbList.at(position), sbList.at(position + 1), 2, 50, 0.0f);
				m_physicsScene->AddActor(spring1);
			}

			if (position + dimX < dimX * dimY) { // links along the x axis
				Spring* spring2 = new Spring(sbList.at(position), sbList.at(position + dimX), 2, 50, 0.0f);
				m_physicsScene->AddActor(spring2);
			}

			// links along the upward diagonal
			if (position + dimX < dimX * dimY && position + dimX + 1 != (x + 2) * dimX) {
				Spring* spring2 = new Spring(sbList.at(position), sbList.at(position + dimX + 1), 2, 50, 0.0f);
				m_physicsScene->AddActor(spring2);
			}

			// links along the downward diagonal
			if (position + dimX < dimX * dimY && position != x * dimX) {
				Spring* spring2 = new Spring(sbList.at(position), sbList.at(position + dimX - 1), 2, 50, 0.0f);
				m_physicsScene->AddActor(spring2);
			}
		}
	}
}

bool Application2D::startup() {
	// maximises the line render limit
	aie::Gizmos::create(255U,255U,65535U,65535U);

	m_2dRenderer = new aie::Renderer2D();

	m_font = new aie::Font("./font/consolas.ttf", 32);
	
	m_physicsScene = new PhysicsScene();
	m_physicsScene->SetGravity(glm::vec2(0, -12));
	m_physicsScene->SetTimeStep(0.01f);

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

	aie::Input* input = aie::Input::getInstance();

	aie::Gizmos::clear();

	// if the scene is unpaused, update the scene
	if (running) { // if running is true, update the scene
		m_physicsScene->Update(deltaTime);
		std::cout << m_physicsScene->GetEnergy() << std::endl;
	}

	// whent he user clicks, decides on what action to take
	if (input->wasMouseButtonPressed(0)) {
		float mouseX = (float)input->getMouseX();
		float mouseY = (float)input->getMouseY();
		glm::vec2 coords = WorldToGizmo(glm::vec2(mouseX, mouseY)); // mouse coordinates for object spawning

		if (spawnerID == 1) { // object 1, sphere
			Sphere * mSphere = new Sphere(coords, glm::vec2(0, 0), 30, 4, glm::vec4(0, 0, 1, 1), 1.0f, spawnKinematic);
			m_physicsScene->AddActor(mSphere);

		} else if (spawnerID == 2) { // object 2, box
			Box * mBox = new Box(coords, glm::vec2(3, 3), glm::vec2(0, 0), 30, spawnKinematic, glm::vec4(0, 1, 0, 1));
			m_physicsScene->AddActor(mBox);

		} else if (spawnerID == 3) { // object 3, soft body
			CreateSoftBody(coords);

		} else if (spawnerID == 4) { // object 4, rope
			if (secondRopeNode) {
				CreateRope(ropeStartPos, coords);
				secondRopeNode = false;
			} else {
				ropeStartPos = coords;
				secondRopeNode = true;
			}
		}
	}

	if (input->isMouseButtonDown(1)) { // deletes rigidbodies within a small radius
		float mouseX = (float)input->getMouseX();
		float mouseY = (float)input->getMouseY();
		glm::vec2 coords = WorldToGizmo(glm::vec2(mouseX, mouseY));
		m_physicsScene->RemoveObject(coords, 5);
	}

	// changes between the spawned object
	if (input->wasKeyPressed(aie::INPUT_KEY_1)) {
		spawnerID = 1;
		secondRopeNode = false;
	} else if (input->wasKeyPressed(aie::INPUT_KEY_2)) {
		spawnerID = 2;
		secondRopeNode = false;
	} else if (input->wasKeyPressed(aie::INPUT_KEY_3)) {
		spawnerID = 3;
		secondRopeNode = false;
	}
	else if (input->wasKeyPressed(aie::INPUT_KEY_4)) {
		spawnerID = 4;
		secondRopeNode = false;
	} else if (input->wasKeyPressed(aie::INPUT_KEY_W)) { // toggles spawn kinematics
		if (spawnKinematic) {
			spawnKinematic = false;
		} else {
			spawnKinematic = true;
		}
	} else if (input->wasKeyPressed(aie::INPUT_KEY_SPACE)) { // pauses/resumes the simulation
		if (running) {
			running = false;
		} else {
			running = true;
		}
	} else if (input->isKeyDown(aie::INPUT_KEY_ESCAPE)) { // exits the simulation
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
