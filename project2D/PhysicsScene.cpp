#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"

class Sphere;
class Plane;
class Box;

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

	Box* box = dynamic_cast<Box*>(obj1);
	Plane* plane = dynamic_cast<Plane*>(obj2);

	// if we are successful then test for collision
	if (box != nullptr && plane != nullptr) {
		int numContacts = 0;
		glm::vec2 contact(0, 0);
		float contactV = 0;
		float radius = 0.5f * std::fminf(box->GetWidth(), box->GetHeight());

		// which side is the centre of mass on?
		glm::vec2 planeOrigin = plane->GetNormal() * plane->GetDistance();
		float comFromPlane = glm::dot(box->GetPosition() - planeOrigin, plane->GetNormal());

		// check all four corners to see fi we've hit the plane
		for (float x = -box->GetExtents().x; x < box->GetWidth(); x += box->GetWidth()) {
			for (float y = -box->GetExtents().y; y < box->GetHeight(); y += box->GetHeight()) {
				// get the position of the cotner in world space
				glm::vec2 p = box->GetPosition() + x * box->GetLocalX() + y * box->GetLocalY();

				float distFromPlane = glm::dot(p - planeOrigin, plane->GetNormal());

				// this is the total velocity of the point
				float velocityIntoPlane = glm::dot(box->GetVelocity() + box->getRotation() * (-y * box->GetLocalX() + x * box->GetLocalY()), plane->GetNormal());

				// if this corner is on teh opposite side from the COM, and moving further in, we need to resolve the collision
				if ((distFromPlane > 0 && comFromPlane < 0 && velocityIntoPlane > 0) || (distFromPlane < 0 && comFromPlane > 0 && velocityIntoPlane < 0)) {
					numContacts++;
					contact += p;
					contactV += velocityIntoPlane;
				}
			}
		}

		// we've had a hit - typically only two crners can contact
		if (numContacts > 0) {
			// get the average collision velocity into the plane (covers linear and rotational velocity of all corners involvde)
			float collisionV = contactV / (float)numContacts;

			// get the acceleration required to stop (restitution = 0) or reverse (restitution = 1) the average velocity into the plane
			glm::vec2 acceleration = -plane->GetNormal() * ((1.0f + box->GetElasticity()) * collisionV);
			
			// and the average position at which we'll apply the force (corner or edge centre)
			glm::vec2 localContact = (contact / (float)numContacts) - box->GetPosition();

			// this is the perpendicular distance we apply the forve at relative to the COM, so torque = F*r
			float r = glm::dot(localContact, glm::vec2(plane->GetNormal().y, -plane->GetNormal().x));

			// work out the "effective mass" - this is a combination of moment of inertia and mass, and tells us how much the contact point velocity will change the force we're applying
			float mass0 = 1.0f / (1.0f / box->getMass() + (r*r) / box->GetMoment());
			
			// and apply the force
			box->ApplyForce(acceleration * mass0, localContact);
		}
	}
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