#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Spring.h"

class Sphere;
class Plane;
class Box;
class Spring;

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
	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;
	CheckForCollision();
	for (auto pActor : m_actors) {
		pActor->FixedUpdate(m_gravity, m_timeStep);
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
	int actorCount = (int)m_actors.size();

	// need to check for collisions against all objects except this one
	for (int outer = 0; outer < actorCount - 1; outer++) {
		for (int inner = outer + 1; inner < actorCount; inner++) {
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];
			int shapeId1 = object1->GetShapeID();
			int shapeId2 = object2->GetShapeID();

			if (shapeId1< 0 || shapeId2 < 0) {
				continue;
			}

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
	return Sphere2Plane(obj2, obj1);
}

bool PhysicsScene::Plane2Box(PhysicsObject* obj1, PhysicsObject* obj2) {
	return Box2Plane(obj1, obj2);
}

bool PhysicsScene::Sphere2Plane(PhysicsObject* obj1, PhysicsObject* obj2) {
	Sphere* sphere = dynamic_cast<Sphere*>(obj1);
	Plane* plane = dynamic_cast<Plane*>(obj2);

	// if successful test for collision
	if (sphere != nullptr && plane != nullptr) {
		glm::vec2 collisionNormal = plane->GetNormal();
		float sphereToPlane = glm::dot(sphere->GetPosition(), plane->GetNormal()) - plane->GetDistance();
		
		// which side is the centre of mass on?
		glm::vec2 planeOrigin = plane->GetNormal() * plane->GetDistance();
		float comFromPlane = glm::dot(sphere->GetPosition() - planeOrigin, plane->GetNormal());

		// if behind plane, flip normal
		if (sphereToPlane < 0) {
			collisionNormal *= -1;
			sphereToPlane *= -1;

		}

		float intersection = sphere->GetRadius() - sphereToPlane;
		if (intersection > 0) {
			glm::vec2 contact = sphere->GetPosition() + (collisionNormal * -sphere->GetRadius());

			plane->ResolveCollision(sphere, contact);

			sphere->Nudge(collisionNormal * intersection);

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
	return Box2Sphere(obj2, obj1);
}

bool PhysicsScene::Box2Plane(PhysicsObject* obj1, PhysicsObject* obj2) {
	Box *box = dynamic_cast<Box*>(obj1);
	Plane *plane = dynamic_cast<Plane*>(obj2);

	float penetration = 0;

	// if we are successful then test for collision
	if (box != nullptr && plane != nullptr) {
		int numContacts = 0;
		glm::vec2 contact(0, 0);
		float contactV = 0;
		float radius = 0.5f * std::fminf(box->GetWidth(), box->GetHeight());

		// which side is the centre of mass on?
		glm::vec2 planeOrigin = plane->GetNormal() * plane->GetDistance();
		float comFromPlane = glm::dot(box->GetPosition() - planeOrigin, plane->GetNormal());

		// check all four corners to see if we've hit the plane
		for (float x = -box->GetExtents().x; x<box->GetWidth(); x += box->GetWidth()) {
			for (float y = -box->GetExtents().y; y<box->GetHeight(); y += box->GetHeight()) {
				// get the position of the corner in world space
				glm::vec2 p = box->GetPosition() + x * box->GetLocalX() + y * box->GetLocalY();
				float distFromPlane = glm::dot(p - planeOrigin, plane->GetNormal());

				// this is the total velocity of the point
				glm::vec2 pointVelocity = box->GetVelocity() + box->GetAngularVelocity() * (-y * box->GetLocalX() + x * box->GetLocalY());
				float velocityIntoPlane = glm::dot(pointVelocity, plane->GetNormal());

				// if this corner is on the opposite side from the COM, and moving further in, we need to resolve the collision
				if ((distFromPlane > 0 && comFromPlane < 0 && velocityIntoPlane > 0) || (distFromPlane < 0 && comFromPlane > 0 && velocityIntoPlane < 0)) {
					numContacts++;
					contact += p;
					contactV += velocityIntoPlane;

					if (comFromPlane >= 0) {
						if (penetration > distFromPlane) penetration = distFromPlane;
					} else if (penetration < distFromPlane) penetration = distFromPlane;
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

			// this is the perpendicular distance we apply the force at relative to the COM, to Torque = F*r
			float r = glm::dot(localContact, glm::vec2(plane->GetNormal().y, -plane->GetNormal().x));

			// work out the "effective mass"  this is  a combination of moment of inertia and mass, and tells us how much the contact point velocity will change with the force we're applying
			float mass0 = 1.0f / (1.0f / box->GetMass() + (r*r) / box->GetMoment());

			// and appky the force
			box->ApplyForce(acceleration * mass0, localContact);

			box->Nudge(-plane->GetNormal() * penetration);
		}
	}
	return false;
}

bool PhysicsScene::Box2Sphere(PhysicsObject* obj1, PhysicsObject* obj2) {
	Box* box = dynamic_cast<Box*>(obj1);
	Sphere* sphere = dynamic_cast<Sphere*>(obj2);

	if (box != nullptr && sphere != nullptr) {
		glm::vec2 circlePos = sphere->GetPosition() - box->GetPosition();
		float w2 = box->GetWidth() / 2, h2 = box->GetHeight() / 2;

		int numContacts = 0;
		glm::vec2 contact(0, 0); // contact is in our box coordinates
		glm::vec2 penVec(0, 0);

		// check the four corners to see if any of them are inside the circle
		for (float x = -w2; x <= w2; x += box->GetWidth()) {
			for (float y = -h2; y <= h2; y += box->GetHeight()) {
				glm::vec2 p = x * box->GetLocalX() + y * box->GetLocalY();
				glm::vec2 dp = p - circlePos;
				if (dp.x * dp.x + dp.y * dp.y < sphere->GetRadius() * sphere->GetRadius()) {
					numContacts++;
					contact += glm::vec2(x, y);
				}
			}
		}
		glm::vec2* direction = nullptr;
		// get the local position of the circle centre
		glm::vec2 localPos(glm::dot(box->GetLocalX(), circlePos), glm::dot(box->GetLocalY(), circlePos));
		if (localPos.y < h2 && localPos.y > -h2) {
			if (localPos.x > 0 && localPos.x < w2 + sphere->GetRadius()) {
				numContacts++;
				contact += glm::vec2(w2, localPos.y);
				direction = new glm::vec2(box->GetLocalX());
			}
			if (localPos.x < 0 && localPos.x > -(w2 + sphere->GetRadius())) {
				numContacts++;
				contact += glm::vec2(-w2, localPos.y);
				direction = new glm::vec2(-box->GetLocalX());
			}
		}
		if (localPos.x < w2 && localPos.x > -w2) {
			if (localPos.y > 0 && localPos.y < h2 + sphere->GetRadius()) {
				numContacts++;
				contact += glm::vec2(localPos.x, h2);
				direction = new glm::vec2(box->GetLocalY());
			}
			if (localPos.y < 0 && localPos.y > -(h2 + sphere->GetRadius())) {
				numContacts++;
				contact += glm::vec2(localPos.x, -h2);
				direction = new glm::vec2(-box->GetLocalY());
			}
		}
		if (numContacts > 0) {
			// average, and convert back into world coords
			contact = box->GetPosition() + (1.0f / numContacts) * (box->GetLocalX() * contact.x + box->GetLocalY()*contact.y);

			// with the contact point we can find a penetration vector


			float distToContact = glm::length(contact - sphere->GetPosition());
			float pen = sphere->GetRadius() - distToContact;
			// fix for strange situation where contact point lies exactly on the sphere centre
			if (distToContact == 0) {
				penVec = glm::normalize(sphere->GetVelocity()) * pen;
			} else {
				penVec = glm::normalize(contact - sphere->GetPosition()) * pen;
			}

			box->ResolveCollision(sphere, contact, direction);	
			
			// move each shape away in the direction of penetration
			if (!box->IsKinematic() && !sphere->IsKinematic()) {
				box->Nudge(penVec * 0.5f);
				sphere->Nudge(-penVec * 0.5f);
			} else if (!box->IsKinematic()) {
				box->Nudge(penVec);
			} else {
				sphere->Nudge(-penVec);
			}
		}
		delete direction;
	}
	return false;
}

bool PhysicsScene::Box2Box(PhysicsObject* obj1, PhysicsObject* obj2) {
	Box* box1 = dynamic_cast<Box*>(obj1);
	Box* box2 = dynamic_cast<Box*>(obj2);

	if (box1 != nullptr && box2 != nullptr) {
		glm::vec2 boxPos = box2->GetPosition() - box1->GetPosition();

		glm::vec2 norm(0, 0);
		glm::vec2 contact(0, 0);
		float pen = 0;
		int numContacts = 0;

		box1->CheckBoxCorners(*box2, contact, numContacts, pen, norm);

		if (box2->CheckBoxCorners(*box1, contact, numContacts, pen, norm)) {
			norm = -norm;
		}

		if (pen > 0){
			box1->ResolveCollision(box2, contact / float(numContacts), &norm);

			// apply contact forces
			glm::vec2 displacement = pen * norm;
			box1->Nudge(-displacement * 0.5f);
			box2->Nudge(displacement * 0.5f);
		}
			return true;
	}
	return false;
}

float PhysicsScene::GetEnergy() {
	float energy = 0;
	for (auto pActor : m_actors) {
		energy += pActor->GetTotalEnergy(m_gravity);
	}
	return energy;
}

void PhysicsScene::DebugScene() {
	int count = 0;
	for (auto pActor : m_actors) {
		std::cout << count << " : ";
		count++;
	}
}