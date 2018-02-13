#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"

class Sphere;
class Plane;

PhysicsScene::PhysicsScene() : m_timeStep(0.01f), m_gravity(glm::vec2 (0,0)) {}

PhysicsScene::~PhysicsScene() {
	for (auto pActor : m_actors) {
		delete pActor;
	}
}

void PhysicsScene::AddActor(PhysicsObject * actor) {
	m_actors.push_back(actor);
}

void PhysicsScene::RemoveActor(PhysicsObject * actor) {
	for (auto iter = m_actors.begin(); iter != m_actors.end(); iter++) {
		if (*iter == actor) {
			m_actors.erase(iter);
		}
	}
}

void PhysicsScene::Update(float deltaTime) {

	static std::list<PhysicsObject*> dirty;

	// update physics at a fixed time step

	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;

	while (accumulatedTime >= m_timeStep) {
		CheckForCollision();
		for (auto pActor : m_actors) {
			pActor->FixedUpdate(m_gravity, m_timeStep);
		}
		accumulatedTime -= m_timeStep;
	}
}

void PhysicsScene::UpdateGizmo() {
	for (auto pActor : m_actors) {
		pActor->Draw();
	}
}

// function pointer array for doing our collisions
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] = {
	PhysicsScene::Plane2Plane, PhysicsScene::Plane2Sphere, PhysicsScene::Plane2Box,
	PhysicsScene::Sphere2Plane, PhysicsScene::Sphere2Sphere,PhysicsScene::Sphere2Box,
	PhysicsScene::Box2Plane, PhysicsScene::Box2Sphere,PhysicsScene::Box2Box
	};

void PhysicsScene::CheckForCollision() {
	int actorCount = m_actors.size();

	// need to check for collisions against all objects except this one
	for (int outer = 0; outer < actorCount - 1; outer++) {
		for (int inner = outer + 1; inner < actorCount; inner++) {
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];
			int shapeId1 = object1->GetShapeID();
			int shapeId2 = object2->GetShapeID();

			// using funciton pointers
			int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
			fn collisionFunctionsPtr = collisionFunctionArray[functionIdx];
			if (collisionFunctionsPtr != nullptr) {
				// did a collision occur?
				collisionFunctionsPtr(object1, object2);
			}
		}
	}
}

bool PhysicsScene::Plane2Plane (PhysicsObject* obj1, PhysicsObject* obj2) {

	return false;
}

bool PhysicsScene::Plane2Sphere(PhysicsObject* obj1, PhysicsObject* obj2) {

	return false;
}

bool PhysicsScene::Plane2Box(PhysicsObject* obj1, PhysicsObject* obj2) {
	return false;
}

bool PhysicsScene::Sphere2Plane(PhysicsObject* obj1, PhysicsObject* obj2) {
	Sphere* sphere = dynamic_cast<Sphere*>(obj1);
	Plane* plane = dynamic_cast<Plane*>(obj2);

	// if successful test for collision
	if (sphere != nullptr && plane != nullptr) {
		glm::vec2 collisionNormal = plane->GetNormal();
		float sphereToPlane = glm::dot(sphere->GetPosition(), plane->GetNormal()) - plane->GetDistance();
		
		// if behind plane, flip normal
		if (sphereToPlane < 0) {
			//collisionNormal *= -1;
			sphereToPlane *= -1;
		}
		float intersection = sphere->GetRadius() - sphereToPlane;
		if (intersection > 0) {
			glm::vec2 contact = sphere->GetPosition() + (collisionNormal * -sphere->GetRadius());
			plane->ResolveCollision(sphere, contact);
		}
	}
	return false;
}

bool PhysicsScene::Sphere2Sphere(PhysicsObject* obj1, PhysicsObject* obj2) {
	// try to cast objects to sphere and sphere
	Sphere* sphere1 = dynamic_cast<Sphere*>(obj1);
	Sphere* sphere2 = dynamic_cast<Sphere*>(obj2);

	// if we are successful then test for collision
	if (sphere1 != nullptr && sphere2 != nullptr) {
		if (glm::distance(sphere1->GetPosition(), sphere2->GetPosition()) <= sphere1->GetRadius() + sphere2->GetRadius()) {
			sphere1->ResolveCollision(sphere2, 0.5f * (sphere1->GetPosition() + sphere2->GetPosition()));
		}
	}
	return false;
}

bool PhysicsScene::Sphere2Box(PhysicsObject* obj1, PhysicsObject* obj2) {

	return false;
}

bool PhysicsScene::Box2Plane(PhysicsObject* obj1, PhysicsObject* obj2) {

	return false;
}

bool PhysicsScene::Box2Sphere(PhysicsObject* obj1, PhysicsObject* obj2) {

	return false;
}

bool PhysicsScene::Box2Box(PhysicsObject* obj1, PhysicsObject* obj2) {

	return false;
}

void PhysicsScene::DebugScene() {
	int count = 0;
	for (auto pActor : m_actors) {
		std::cout << count << " : ";
		count++;
	}
}