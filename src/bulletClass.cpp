#include "bulletClass.hpp"

#if USE_PHYSICS_DEBUG_DRAW
#include "physicsDebugDraw.hpp"
#endif
#include "Ragdoll.hpp"
// Infra.
#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include <math.h>

PhysicsObjectClass::PhysicsObjectClass()
{
    initializePhysics(); // I like to keep the initialization separate from the constructor
}

// This is the destructor for the PhysicsObjectClass to clean up the memory
PhysicsObjectClass::~PhysicsObjectClass()
{
#if USE_PHYSICS_DEBUG_DRAW
	delete physicsDebugDrawObject;	
#endif
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
}

void PhysicsObjectClass::initializePhysics()
{
    // Basic setup for Bullet Physics
    // Perhaps the most important part of this for N64 applications is the pool size
    // Due to the limited ram, we need to keep the pool size small
    // This should keep the size of the pool to 1MB total.
    // The default pool size is way too large for N64 applications and will throw an error.
    btDefaultCollisionConstructionInfo constructionInfo;
    constructionInfo.m_defaultMaxCollisionAlgorithmPoolSize = 512;
    constructionInfo.m_defaultMaxPersistentManifoldPoolSize = 512;

    collisionConfiguration = new btDefaultCollisionConfiguration(constructionInfo);
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    setGravity(btVector3(0, -30, 0)); // Set gravity to -30 on the Y axis to start
	
#if USE_PHYSICS_DEBUG_DRAW
	physicsDebugDrawObject = new PhysicsDebugDraw();
	dynamicsWorld->setDebugDrawer(physicsDebugDrawObject);
#endif
}

void PhysicsObjectClass::stepSimulation(float deltaTime)
{
    dynamicsWorld->stepSimulation(1.f / deltaTime, 10); // Step the simulation at 30fps
}

void PhysicsObjectClass::setGravity(btVector3 gravity)
{
    dynamicsWorld->setGravity(gravity);
}

btVector3 PhysicsObjectClass::getPrismRigidBodyPosition(btRigidBody *prismRigidBody)
{
    btTransform trans;
    prismRigidBody->getMotionState()->getWorldTransform(trans); // Use the passed prismBody instead of GetPrismRigidBody()

    btVector3 pos = trans.getOrigin();

    return pos;
}

btQuaternion PhysicsObjectClass::getPrismRigidBodyRotation(btRigidBody *prismRigidBody)
{
    btTransform trans;

    prismRigidBody->getMotionState()->getWorldTransform(trans);

    btQuaternion rot = trans.getRotation();

    return rot;
}

void PhysicsObjectClass::createPrismRigidBody(int rigidBodySize, int startingHeight, btVector3 startPosition)
{
    // Similar to CreatePrism, but use the x, y, z parameters for the position
    // Create prism shape and body
    btConvexHullShape *prismShape = new btConvexHullShape();
    // Define vertices for the prism shape here

    // Front Face
    prismShape->addPoint(btVector3(0, rigidBodySize * 3, 0));
    prismShape->addPoint(btVector3(-rigidBodySize, -rigidBodySize, rigidBodySize));
    prismShape->addPoint(btVector3(rigidBodySize, -rigidBodySize, rigidBodySize));

    // Right Face
    prismShape->addPoint(btVector3(0, rigidBodySize * 3, 0));
    prismShape->addPoint(btVector3(rigidBodySize, -rigidBodySize, rigidBodySize));
    prismShape->addPoint(btVector3(rigidBodySize, -rigidBodySize, -rigidBodySize));

    // Back Face
    prismShape->addPoint(btVector3(0, rigidBodySize * 3, 0));
    prismShape->addPoint(btVector3(rigidBodySize, -rigidBodySize, -rigidBodySize));
    prismShape->addPoint(btVector3(-rigidBodySize, -rigidBodySize, -rigidBodySize));

    // Left Face
    prismShape->addPoint(btVector3(0, rigidBodySize * 3, 0));
    prismShape->addPoint(btVector3(-rigidBodySize, -rigidBodySize, -rigidBodySize));
    prismShape->addPoint(btVector3(-rigidBodySize, -rigidBodySize, rigidBodySize));

    // btCollisionShape *prismShape = new btBoxShape(btVector3(rigidBodySize, rigidBodySize, rigidBodySize)); // Example for a box-shaped prism

    // Set the starting position of the prism
    btDefaultMotionState *prismMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, startingHeight, 0)));

    btScalar mass = 5; // Mass of the prism (in kg) - raise to make the prism heavier
    btVector3 prismInertia(0, 0, 0);
    prismShape->calculateLocalInertia(mass, prismInertia);                                                       // Calculate the inertia of the prism
    btRigidBody::btRigidBodyConstructionInfo prismRigidBodyCI(mass, prismMotionState, prismShape, prismInertia); // I made the mass 100 so the prism is heavier
    btRigidBody *prismRigidBody = new btRigidBody(prismRigidBodyCI);                                             // Create the prism rigid body
    dynamicsWorld->addRigidBody(prismRigidBody);                                                                 // Add the prism to the physics world
    prismRigidBody->setRestitution(0.3f);                                                                        // Make the prism bouncy
    prismRigidBody->setFriction(0.5f);                                                                           // Make the prism slippery
    prismRigidBody->applyTorque(btVector3(1, 1, 5));                                                             // Zero out the torque to start

    // Apply an initial angular velocity to make the prism spin
    prismRigidBody->setAngularVelocity(btVector3(1, 2, 1));

    prismRigidBodies.push_back(prismRigidBody);
}

void PhysicsObjectClass::DebugDrawWorld(int shouldDraw)
{
    if (shouldDraw)
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_DEPTH_TEST);

        glBegin(GL_LINES);

        dynamicsWorld->debugDrawWorld();

        glEnd();
        glPopMatrix();
    }
}

void PhysicsObjectClass::resetPrismRigidBodies()
{
    // Reset the prism rigid body to the starting position
    // prismRigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 100, 0))); //Legacy code for just one prism

    for (auto *rigidBody : prismRigidBodies)
    {
        rigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 100, 0)));
        rigidBody->setLinearVelocity(btVector3(0, 0, 0));
        rigidBody->setAngularVelocity(btVector3(0, 0, 0));
    }
}

void PhysicsObjectClass::createGroundRigidBody()
{
    // Define ground shape and body - this is for an infinite ground, but I want to let the user define the size of the ground
    // btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

    // Plane dimensions
    float planeWidth = 200.0f; // Width of the plane
    float planeDepth = 200.0f; // Depth of the plane

    // Define ground shape as a box - since I want the user to rotate the plane, I am making it a kinematic object
    btVector3 groundHalfExtents(planeWidth / 2, 0.1f, planeDepth / 2);
    btCollisionShape *groundShape = new btBoxShape(groundHalfExtents);

    btDefaultMotionState *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

    // Zero mass for kinematic objects - it will not move unless you move it
    btScalar mass = 0;
    btVector3 groundInertia(0, 0, 0);
    groundShape->calculateLocalInertia(mass, groundInertia);

    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass, groundMotionState, groundShape, groundInertia);
    groundRigidBody = new btRigidBody(groundRigidBodyCI); // Create the ground rigid body
    groundRigidBody->setRestitution(1.0f);                // Make the ground bouncy
    // Set the ground as a kinematic object
    groundRigidBody->setCollisionFlags(groundRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT); // Set the ground as a kinematic object - this allows us to rotate the ground, but it does not allow the ground to bounce off of other objects
    groundRigidBody->setActivationState(DISABLE_DEACTIVATION);                                                         // Make the ground always active

    dynamicsWorld->addRigidBody(groundRigidBody); // Add the ground to the physics world
}

void PhysicsObjectClass::updatePlaneRotation(float rotationX, float rotationZ)
{
    btTransform trans;
    groundRigidBody->getMotionState()->getWorldTransform(trans);

    // Convert degrees to radians for Bullet Physics
    float radianX = rotationX * SIMD_PI / 180.0f; // Use SIMD_PI instead of M_PI
    float radianZ = rotationZ * SIMD_PI / 180.0f; // Use SIMD_PI instead of M_PI

    // Set rotation using Euler angles
    btQuaternion quat;
    quat.setEuler(0, radianX, radianZ); // Assuming rotation around Z and X axes
    trans.setRotation(quat);

    groundRigidBody->getMotionState()->setWorldTransform(trans);
    groundRigidBody->setWorldTransform(trans); // Update the world transform as well
}

void PhysicsObjectClass::createRagdoll()
{
    RagDoll* ragdollInstance = new RagDoll();
    ragdollInstance->Initialize(btVector3(0.0f, 10.0f, 0.0f), 10.0f);
    AddAssembly(ragdollInstance);

    ragdolls.push_back(ragdollInstance);
}

void PhysicsObjectClass::createBox()
{
    static const float kBoxSize = 2.5f;
    btBoxShape* boxShape = new btBoxShape(btVector3(kBoxSize, kBoxSize, kBoxSize));
    btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10.0f, 0)));

    // Zero mass for kinematic objects - it will not move unless you move it
    btScalar mass = 1.0;
    btVector3 boxInertia(0, 0, 0);
    boxShape->calculateLocalInertia(mass, boxInertia);

    btRigidBody::btRigidBodyConstructionInfo boxCreateInfo(mass, boxMotionState, boxShape, boxInertia);
    btRigidBody* boxRigidBody = new btRigidBody(boxCreateInfo); // Create the ground rigid body

    dynamicsWorld->addRigidBody(boxRigidBody); // Add the ground to the physics world
}

void PhysicsObjectClass::createSphere()
{
    static const float kSphereRadius = 1.5f;
    btSphereShape* sphereShape = new btSphereShape(kSphereRadius);
    btDefaultMotionState* sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10.0f, 0)));

    // Zero mass for kinematic objects - it will not move unless you move it
    btScalar mass = 1.0;
    btVector3 sphereInertia(0, 0, 0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);

    btRigidBody::btRigidBodyConstructionInfo sphereCreateInfo(mass, sphereMotionState, sphereShape, sphereInertia);
    btRigidBody* sphereRigidBody = new btRigidBody(sphereCreateInfo); // Create the ground rigid body

    dynamicsWorld->addRigidBody(sphereRigidBody); // Add the ground to the physics world
}

void PhysicsObjectClass::AddRigidBody(btRigidBody* rigidBody)
{
    assert(rigidBody != nullptr);
    if (rigidBody != nullptr)
    {
        dynamicsWorld->addRigidBody(rigidBody);
    }
}

void PhysicsObjectClass::RemoveRigidBody(btRigidBody* rigidBody)
{
    assert(rigidBody != nullptr);
    if (rigidBody != nullptr)
    {
        dynamicsWorld->removeRigidBody(rigidBody);
    }
}

void PhysicsObjectClass::AddCollider(btCollisionObject* collider)
{
    assert(collider != nullptr);
    if (collider != nullptr)
    {
        dynamicsWorld->addCollisionObject(collider, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::CharacterFilter);
    }
}

void PhysicsObjectClass::RemoveCollider(btCollisionObject* collider)
{
    assert(collider != nullptr);
    if (collider != nullptr)
    {
        dynamicsWorld->removeCollisionObject(collider);
    }
}

void PhysicsObjectClass::AddConstraint(btTypedConstraint* constraint)
{
    assert(constraint != nullptr);
    if (constraint != nullptr)
    {
        bool disableConstraintsBetweenLinkedBodies = true;
        dynamicsWorld->addConstraint(constraint, disableConstraintsBetweenLinkedBodies);
    }
}

void PhysicsObjectClass::RemoveConstraint(btTypedConstraint* constraint)
{
    assert(constraint != nullptr);
    if (constraint != nullptr)
    {
        dynamicsWorld->removeConstraint(constraint);
    }
}

void PhysicsObjectClass::AddAssembly(PhysicalAssembly* assembly)
{
    assert(assembly != nullptr);
    if (assembly != nullptr)
    {
        // Add all of the assembly's collision objects to the world.
        const std::vector<btCollisionObject*>& assemblyColliders = assembly->GetColliders();
        for (size_t colliderIndex = 0; colliderIndex < assemblyColliders.size(); colliderIndex++)
        {
            AddCollider(assemblyColliders[colliderIndex]);
        }

        // Add all of the assembly's rigid bodies to the world.
        const std::vector<btRigidBody*>& assemblyRigidBodies = assembly->GetRigidBodies();
        for (size_t bodyIndex = 0; bodyIndex < assemblyRigidBodies.size(); bodyIndex++)
        {
            AddRigidBody(assemblyRigidBodies[bodyIndex]);
        }

        // Add all of the assembly's constraints to the world.
        const std::vector<btTypedConstraint*>& assemblyConstraints = assembly->GetConstraints();
        for (size_t constraintIndex = 0; constraintIndex < assemblyConstraints.size(); constraintIndex++)
        {
            AddConstraint(assemblyConstraints[constraintIndex]);
        }

        // Add all of the assembly's actions.
        const std::vector<btActionInterface*>& assemblyActions = assembly->GetActions();
        for (size_t actionIndex = 0; actionIndex < assemblyActions.size(); actionIndex++)
        {
            AddAction(assemblyActions[actionIndex]);
        }
    }
}

void PhysicsObjectClass::RemoveAssembly(PhysicalAssembly* assembly)
{
    assert(assembly != nullptr);
    if (assembly != nullptr)
    {
        // Remove all of the assembly's collision objects from the world.
        const std::vector<btCollisionObject*>& assemblyColliders = assembly->GetColliders();
        for (size_t colliderIndex = 0; colliderIndex < assemblyColliders.size(); colliderIndex++)
        {
            RemoveCollider(assemblyColliders[colliderIndex]);
        }

        // Remove all of the assembly's rigid bodies to the world.
        const std::vector<btRigidBody*>& assemblyRigidBodies = assembly->GetRigidBodies();
        for (size_t bodyIndex = 0; bodyIndex < assemblyRigidBodies.size(); bodyIndex++)
        {
            RemoveRigidBody(assemblyRigidBodies[bodyIndex]);
        }

        // Remove all of the assembly's constraints to the world.
        const std::vector<btTypedConstraint*>& assemblyConstraints = assembly->GetConstraints();
        for (size_t constraintIndex = 0; constraintIndex < assemblyConstraints.size(); constraintIndex++)
        {
            RemoveConstraint(assemblyConstraints[constraintIndex]);
        }

        // Remove all of the assembly's actions to the world.
        const std::vector<btActionInterface*>& assemblyActions = assembly->GetActions();
        for (size_t actionIndex = 0; actionIndex < assemblyActions.size(); actionIndex++)
        {
            RemoveAction(assemblyActions[actionIndex]);
        }
    }
}

void PhysicsObjectClass::AddAction(btActionInterface* action)
{
    dynamicsWorld->addAction(action);
}

void PhysicsObjectClass::RemoveAction(btActionInterface* action)
{
    dynamicsWorld->removeAction(action);
}
