#pragma once

#include "../components/Physics.h"
#include "../components/PhysicsCharacter.h"
#include "../components/Transform.h"
#include "../System.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <map>

struct RayCastResult {
	bool hasHit;
	Entity *entity;
	btVector3 position;
};

class PhysicsSystem : public System {
public:
	static const SystemType TYPE = SYSTEM_PHYSICS;

	PhysicsSystem();
	~PhysicsSystem();

	void update(double deltaTime);

	SystemType type() const;

	RayCastResult rayCast(const btVector3 &from, const btVector3 &to);

private:
	static const int TYPE_PHYSICS_CHARACTER = PhysicsCharacter::TYPE | Transform::TYPE;


	static const float CHARACTER_HEIGHT;

	// The vertical offset of the eye from the center of the character
	static const float CHARACTER_EYE_OFFSET;

	struct PhysicsCache {
		btRigidBody *rigidBody;
	};

	struct PhysicsCharacterCache {
		btKinematicCharacterController *characterController;
		btConvexShape *collisionShape;
		btPairCachingGhostObject *ghostObject;
	};

	btBroadphaseInterface *broadphase_;
	btGhostPairCallback *ghostPairCallback_;
	btDefaultCollisionConfiguration *collisionConfiguration_;
	btCollisionDispatcher *dispatcher_;
	btSequentialImpulseConstraintSolver *solver_;
	btDiscreteDynamicsWorld *dynamicsWorld_;

	std::map<Entity *, PhysicsCache *> physicsCaches_;
	std::map<Entity *, PhysicsCharacterCache *> physicsCharacterCaches_;

	void addPhysicsCache(Entity *entity);
	void removePhysicsCache(Entity *entity);
	//void updatePhysicsCache(Entity *entity);

	void addPhysicsCharacterCache(Entity *entity);
	void removePhysicsCharacterCache(Entity *entity);

	void onChangeType(Entity *entity, int fromType, int toType);

	void onMessage(MessageType messageType, Entity *entity);

	void onAttach(Game *game);
};