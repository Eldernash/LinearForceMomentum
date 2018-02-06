#include "PhysicsScene.h"
#include "RigidBody.h"

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
		for (auto pActor : m_actors) {
			pActor->FixedUpdate(m_gravity, m_timeStep);
		}
		accumulatedTime -= m_timeStep;

		// check for collisions (ideally you'd want to have some sort of scene management)
		for (auto pActor : m_actors) {
			for (auto pOther : m_actors) {
				if (pActor == pOther)
				continue;
				
				if (std::find(dirty.begin(), dirty.end(), pActor) != dirty.end() && std::find(dirty.begin(), dirty.end(), pOther) != dirty.end())
					continue;

				RigidBody* pRigid = dynamic_cast<RigidBody*>(pActor);
				if (pRigid->CheckCollision(pOther) == true) {
					pRigid->ApplyForceToActor(dynamic_cast<RigidBody*>(pOther), pRigid->GetVelocity() * pRigid->getMass());
					dirty.push_back(pRigid);
					dirty.push_back(pOther);
				}
			}
		}
		dirty.clear();
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
	PhysicsScene::plane2Plane, PhysicsScene::plane2Sphere, PhysicsScene::plane2Box,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,PhysicsScene::sphere2Box,
	PhysicsScene::box2Plane, PhysicsScene::box2Sphere,PhysicsScene::box2Box
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
		}
	}

}

void PhysicsScene::DebugScene() {
	int count = 0;
	for (auto pActor : m_actors) {
		std::cout << count << " : ";
		pActor->Debug();
		count++;
	}
}
