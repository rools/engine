#include "engine/systems/PhysicsSystem.h"

#include "engine/components/MeshShape.h"
#include "engine/Entity.h"

#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

const float PhysicsSystem::CHARACTER_HEIGHT = 1.75;
const float PhysicsSystem::CHARACTER_EYE_OFFSET = 0.725;

PhysicsSystem::PhysicsSystem() {
	broadphase_ = new btDbvtBroadphase();
	ghostPairCallback_ = new btGhostPairCallback();
	broadphase_->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback_);
	collisionConfiguration_ = new btDefaultCollisionConfiguration();
	dispatcher_ = new btCollisionDispatcher(collisionConfiguration_);
	solver_ = new btSequentialImpulseConstraintSolver();
	dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collisionConfiguration_);
	dynamicsWorld_->setGravity(btVector3(0.0f, -9.8f, 0.0f));
	dynamicsWorld_->getDispatchInfo().m_allowedCcdPenetration = 0.00001f;
}

PhysicsSystem::~PhysicsSystem() {
	for (auto physics : physicsCaches_)
		removePhysicsCache(physics.first);

	delete dynamicsWorld_;
	delete solver_;
	delete dispatcher_;
	delete collisionConfiguration_;
	delete broadphase_;
}

void PhysicsSystem::update(double deltaTime) {
	/*for (auto physics : physicsCaches_) {
		Entity *entity = physics.first;
		PhysicsCache *cache = physics.second;
		cache->rigidBody->tra
		entity->get<Transform>()->setTransform(cache->rigidBody->getWorldTransform());
	}*/

	for (auto physicsCharacter : physicsCharacterCaches_) {
		Entity *entity = physicsCharacter.first;
		PhysicsCharacterCache *cache = physicsCharacter.second;
		cache->characterController->setWalkDirection(entity->get<PhysicsCharacter>()->getVelocity());
		//cache->characterController->playerStep(btCollisionWorld *collisionWorld, btScalar dt)

		btTransform transform = entity->get<Transform>()->transform();

		// Don't actually rotate the player in the physics world
		transform.setRotation(btQuaternion(0.0, 0.0, 0.0));

		transform.setOrigin(transform.getOrigin() - btVector3(0.0, CHARACTER_EYE_OFFSET, 0.0));

		cache->ghostObject->setWorldTransform(transform);
	}

	dynamicsWorld_->stepSimulation(deltaTime, 7);

	for (auto physics : physicsCaches_) {
		Entity *entity = physics.first;
		PhysicsCache *cache = physics.second;
		if (cache->rigidBody)
			entity->get<Transform>()->setTransform(cache->rigidBody->getWorldTransform());
	}

	for (auto physicsCharacter : physicsCharacterCaches_) {
		Entity *entity = physicsCharacter.first;
		PhysicsCharacterCache *cache = physicsCharacter.second;

		// Update the transform component from the physics world, but ignore rotation
		btQuaternion currentRotation = entity->get<Transform>()->transform().getRotation();
		btTransform newTransform = btTransform(currentRotation, cache->ghostObject->getWorldTransform().getOrigin() + btVector3(0.0, CHARACTER_EYE_OFFSET, 0.0));
		entity->get<Transform>()->setTransform(newTransform);
	}
}

SystemType PhysicsSystem::type() const {
	return TYPE;
}

RayCastResult PhysicsSystem::rayCast(const btVector3 &from, const btVector3 &to) {
	RayCastResult result;
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

	dynamicsWorld_->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit()) {
		result.hasHit = true;
		result.entity = (Entity *)rayCallback.m_collisionObject->getUserPointer();
		result.position = rayCallback.m_hitPointWorld;
		return result;
	}

	result.hasHit = false;

	return result;
}

void PhysicsSystem::addPhysicsCache(Entity *entity) {
	PhysicsCache *cache = new PhysicsCache();
	cache->rigidBody = NULL;
	physicsCaches_[entity] = cache;

	if (entity->get<MeshShape>()->triangles().size() == 0)
		return;

	btMotionState *motionState = new btDefaultMotionState(entity->get<Transform>()->transform());
	btScalar mass;

	btCollisionShape *collisionShape;

	switch (entity->get<Physics>()->physicsType()) {
		case PHYSICS_STATIC: {
			mass = 0.0;

			btTriangleMesh *triangleMesh = new btTriangleMesh();

			for (const Triangle *triangle : entity->get<MeshShape>()->triangles())
				triangleMesh->addTriangle(triangle->vertices[0]->vector, triangle->vertices[1]->vector, triangle->vertices[2]->vector);

			collisionShape = new btBvhTriangleMeshShape(triangleMesh, true, true);

			break;
		} case PHYSICS_DYNAMIC: {
			// TODO: The mass should not be hard-coded...
			mass = 1.0;

			btConvexHullShape *convexHullShape = new btConvexHullShape();

			// Create initial hull containing all vertices of the mesh
			for (const Vertex *vertex : entity->get<MeshShape>()->vertices())
				convexHullShape->addPoint(vertex->vector);

			// TODO: Only simplify hull if convex hull has too many vertices?

			// Simplify the hull
			btShapeHull *shapeHull = new btShapeHull(convexHullShape);
			shapeHull->buildHull(convexHullShape->getMargin());

			delete convexHullShape;
			convexHullShape = new btConvexHullShape();

			for (int i = 0; i < shapeHull->numVertices(); ++i)
				convexHullShape->addPoint(shapeHull->getVertexPointer()[i]);

			delete shapeHull;

			collisionShape = convexHullShape;

			break;
		}
	}

	btVector3 localInertia(0.0, 0.0, 0.0);
	collisionShape->calculateLocalInertia(mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo terrainRigidBodyCI(mass, motionState, collisionShape, localInertia);
	cache->rigidBody = new btRigidBody(terrainRigidBodyCI);
	cache->rigidBody->setUserPointer(entity);
	dynamicsWorld_->addRigidBody(cache->rigidBody);
}

void PhysicsSystem::removePhysicsCache(Entity *entity) {
	PhysicsCache *cache = physicsCaches_[entity];
	physicsCaches_.erase(entity);

	if (!cache->rigidBody) {
		delete cache;
		return;
	}

	// TODO: This is probably not a valid order to delete stuff...
	dynamicsWorld_->removeRigidBody(cache->rigidBody);
	btMotionState *motionState = cache->rigidBody->getMotionState();
	btCollisionShape *collisionShape = cache->rigidBody->getCollisionShape();
	delete cache->rigidBody;
	delete collisionShape;
	delete motionState;

	delete cache;
}

void PhysicsSystem::addPhysicsCharacterCache(Entity *entity) {
	PhysicsCharacterCache *cache = new PhysicsCharacterCache();
	cache->collisionShape = new btCapsuleShape(0.1f, CHARACTER_HEIGHT);
	cache->ghostObject = new btPairCachingGhostObject();
	cache->ghostObject->setCollisionShape(cache->collisionShape);
	cache->ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	cache->characterController = new btKinematicCharacterController(cache->ghostObject, cache->collisionShape, 0.3);
	dynamicsWorld_->addCollisionObject(cache->ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter | btBroadphaseProxy::DefaultFilter);
	dynamicsWorld_->addAction(cache->characterController);
	cache->ghostObject->setWorldTransform(entity->get<Transform>()->transform());
	physicsCharacterCaches_[entity] = cache;
}

void PhysicsSystem::removePhysicsCharacterCache(Entity *entity) {
	PhysicsCharacterCache *cache = physicsCharacterCaches_[entity];
	dynamicsWorld_->removeAction(cache->characterController);
	dynamicsWorld_->removeCollisionObject(cache->ghostObject);
	delete cache->characterController;
	delete cache->ghostObject;
	delete cache->collisionShape;
}

void PhysicsSystem::onChangeType(Entity *entity, int fromType, int toType) {
	const bool fromPhysics = (fromType & Physics::TYPE) && (fromType & Transform::TYPE) && (fromType & MeshShape::TYPE);
	const bool toPhysics = (toType & Physics::TYPE) && (toType & Transform::TYPE) && (toType & MeshShape::TYPE );

	if (!fromPhysics && toPhysics) {
		// A physics entity was attached
		addPhysicsCache(entity);
		entity->subscribe(MESSAGE_MESH_CHANGED, this);
	} else if (fromPhysics && !toPhysics) {
		// A physics entity was detached
		entity->unsubscribe(MESSAGE_MESH_CHANGED , this);
		removePhysicsCache(entity);
	}

	const bool fromPhysicsCharacter = (fromType & TYPE_PHYSICS_CHARACTER) == TYPE_PHYSICS_CHARACTER;
	const bool toPhysicsCharacter = (toType & TYPE_PHYSICS_CHARACTER) == TYPE_PHYSICS_CHARACTER;

	if (!fromPhysicsCharacter && toPhysicsCharacter) {
		// A physics character entity was attached
		addPhysicsCharacterCache(entity);
	} else if (fromPhysicsCharacter && !toPhysicsCharacter) {
		// A physics character entity was detached
		removePhysicsCharacterCache(entity);
	}
}

void PhysicsSystem::onMessage(MessageType messageType, Entity *entity) {
	removePhysicsCache(entity);
	addPhysicsCache(entity);
}

void PhysicsSystem::onAttach(Game *game) {

}