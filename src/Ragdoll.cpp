// Local.
#include "Ragdoll.hpp"

#ifndef M_PI
#define M_PI btScalar(3.14159265358979323846)
#endif

#ifndef M_PI_2
#define M_PI_2 (M_PI / 2.0f)
#endif

#ifndef M_PI_4
#define M_PI_4 (M_PI_2 / 2.0f)
#endif

void RagDoll::Initialize(const btVector3& positionOffset, btScalar scale)
{
	m_positionOffset = positionOffset;
	m_scale = scale;

	PhysicalAssembly::Initialize();
}

void RagDoll::Cleanup()
{
	// Destroy all constraints
	for (int i = 0; i < JOINT_COUNT; ++i)
	{
		delete m_constraints[i];
		m_constraints[i] = 0;
	}

	// Destroy all bodies and shapes
	for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		delete m_rigidBodies[i]->getMotionState();

		delete m_rigidBodies[i];
		m_rigidBodies[i] = 0;
		delete m_shapes[i];
		m_shapes[i] = 0;
	}
}

class BodyFactory
{
public:
	BodyFactory() = delete;
	~BodyFactory() = delete;

	static btRigidBody* CreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
	{
		btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		// Rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia = {};
		if (isDynamic)
		{
			shape->calculateLocalInertia(mass, localInertia);
		}

		btDefaultMotionState* bodyMotionState = new btDefaultMotionState(startTransform); // Set the starting position of the ground
		btRigidBody::btRigidBodyConstructionInfo bodyInfo(mass, bodyMotionState, shape, localInertia);
		btRigidBody* rigidBody = new btRigidBody(bodyInfo);

		//rigidBody->setUserPointer(parentEntity);

		return rigidBody;
	}
};

void RagDoll::InitializeBodies()
{
	// Resize the shapes array up-front, because we already know how many we're going to use.
	m_shapes.resize(BODYPART_COUNT);
	m_rigidBodies.resize(BODYPART_COUNT);

	// Setup the geometry
	m_shapes[BODYPART_PELVIS] = new btCapsuleShape(btScalar(0.15) * m_scale, btScalar(0.20) * m_scale);
	m_shapes[BODYPART_SPINE] = new btCapsuleShape(btScalar(0.15) * m_scale, btScalar(0.28) * m_scale);
	m_shapes[BODYPART_HEAD] = new btCapsuleShape(btScalar(0.10) * m_scale, btScalar(0.05) * m_scale);
	m_shapes[BODYPART_LEFT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07) * m_scale, btScalar(0.45) * m_scale);
	m_shapes[BODYPART_LEFT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05) * m_scale, btScalar(0.37) * m_scale);
	m_shapes[BODYPART_RIGHT_UPPER_LEG] = new btCapsuleShape(btScalar(0.07) * m_scale, btScalar(0.45) * m_scale);
	m_shapes[BODYPART_RIGHT_LOWER_LEG] = new btCapsuleShape(btScalar(0.05) * m_scale, btScalar(0.37) * m_scale);
	m_shapes[BODYPART_LEFT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05) * m_scale, btScalar(0.33) * m_scale);
	m_shapes[BODYPART_LEFT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04) * m_scale, btScalar(0.25) * m_scale);
	m_shapes[BODYPART_RIGHT_UPPER_ARM] = new btCapsuleShape(btScalar(0.05) * m_scale, btScalar(0.33) * m_scale);
	m_shapes[BODYPART_RIGHT_LOWER_ARM] = new btCapsuleShape(btScalar(0.04) * m_scale, btScalar(0.25) * m_scale);

	// Setup all the rigid bodies
	btTransform offset;
	offset.setIdentity();
	offset.setOrigin(m_positionOffset);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(1.), btScalar(0.)));
	m_rigidBodies[BODYPART_PELVIS] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_PELVIS]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(1.2), btScalar(0.)));
	m_rigidBodies[BODYPART_SPINE] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_SPINE]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(1.6), btScalar(0.)));
	m_rigidBodies[BODYPART_HEAD] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_HEAD]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(-0.18), btScalar(0.65), btScalar(0.)));
	m_rigidBodies[BODYPART_LEFT_UPPER_LEG] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_UPPER_LEG]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(-0.18), btScalar(0.2), btScalar(0.)));
	m_rigidBodies[BODYPART_LEFT_LOWER_LEG] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_LOWER_LEG]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.18), btScalar(0.65), btScalar(0.)));
	m_rigidBodies[BODYPART_RIGHT_UPPER_LEG] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_UPPER_LEG]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.18), btScalar(0.2), btScalar(0.)));
	m_rigidBodies[BODYPART_RIGHT_LOWER_LEG] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_LOWER_LEG]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(-0.35), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0, 0, M_PI_2);
	m_rigidBodies[BODYPART_LEFT_UPPER_ARM] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_UPPER_ARM]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(-0.7), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0, 0, M_PI_2);
	m_rigidBodies[BODYPART_LEFT_LOWER_ARM] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_LEFT_LOWER_ARM]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.35), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0, 0, -M_PI_2);
	m_rigidBodies[BODYPART_RIGHT_UPPER_ARM] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_UPPER_ARM]);

	transform.setIdentity();
	transform.setOrigin(m_scale * btVector3(btScalar(0.7), btScalar(1.45), btScalar(0.)));
	transform.getBasis().setEulerZYX(0, 0, -M_PI_2);
	m_rigidBodies[BODYPART_RIGHT_LOWER_ARM] = BodyFactory::CreateRigidBody(btScalar(1.), offset * transform, m_shapes[BODYPART_RIGHT_LOWER_ARM]);

	// Setup some damping on the m_rigidBodies
	for (int i = 0; i < BODYPART_COUNT; ++i)
	{
		m_rigidBodies[i]->setDamping(btScalar(0.05), btScalar(0.85));
		m_rigidBodies[i]->setDeactivationTime(btScalar(0.8));
		m_rigidBodies[i]->setSleepingThresholds(btScalar(1.6), btScalar(2.5));
	}
}

void RagDoll::InitializeConstraints()
{
	// Resize the constraints array because we know how many we're going to create.
	m_constraints.resize(JOINT_COUNT);

	// Now setup the constraints
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;

	btTransform localA, localB;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, M_PI_2, 0);
	localA.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, M_PI_2, 0);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.15), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_rigidBodies[BODYPART_PELVIS], *m_rigidBodies[BODYPART_SPINE], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_4), btScalar(M_PI_2));
	m_constraints[JOINT_PELVIS_SPINE] = hingeC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, M_PI_2);
	localA.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.30), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_2);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_rigidBodies[BODYPART_SPINE], *m_rigidBodies[BODYPART_HEAD], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, M_PI_2);
	m_constraints[JOINT_SPINE_HEAD] = coneC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, -M_PI_4 * 5);
	localA.setOrigin(m_scale * btVector3(btScalar(-0.18), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, -M_PI_4 * 5);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_rigidBodies[BODYPART_PELVIS], *m_rigidBodies[BODYPART_LEFT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_constraints[JOINT_LEFT_HIP] = coneC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, M_PI_2, 0);
	localA.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, M_PI_2, 0);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_rigidBodies[BODYPART_LEFT_UPPER_LEG], *m_rigidBodies[BODYPART_LEFT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_constraints[JOINT_LEFT_KNEE] = hingeC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, M_PI_4);
	localA.setOrigin(m_scale * btVector3(btScalar(0.18), btScalar(-0.10), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_4);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.225), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_rigidBodies[BODYPART_PELVIS], *m_rigidBodies[BODYPART_RIGHT_UPPER_LEG], localA, localB);
	coneC->setLimit(M_PI_4, M_PI_4, 0);
	m_constraints[JOINT_RIGHT_HIP] = coneC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, M_PI_2, 0);
	localA.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.225), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, M_PI_2, 0);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.185), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_rigidBodies[BODYPART_RIGHT_UPPER_LEG], *m_rigidBodies[BODYPART_RIGHT_LOWER_LEG], localA, localB);
	hingeC->setLimit(btScalar(0), btScalar(M_PI_2));
	m_constraints[JOINT_RIGHT_KNEE] = hingeC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, M_PI);
	localA.setOrigin(m_scale * btVector3(btScalar(-0.2), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_2);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_rigidBodies[BODYPART_SPINE], *m_rigidBodies[BODYPART_LEFT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_2, M_PI_2, 0);
	m_constraints[JOINT_LEFT_SHOULDER] = coneC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, M_PI_2, 0);
	localA.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, M_PI_2, 0);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_rigidBodies[BODYPART_LEFT_UPPER_ARM], *m_rigidBodies[BODYPART_LEFT_LOWER_ARM], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
	m_constraints[JOINT_LEFT_ELBOW] = hingeC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, 0, 0);
	localA.setOrigin(m_scale * btVector3(btScalar(0.2), btScalar(0.15), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, 0, M_PI_2);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.18), btScalar(0.)));
	coneC = new btConeTwistConstraint(*m_rigidBodies[BODYPART_SPINE], *m_rigidBodies[BODYPART_RIGHT_UPPER_ARM], localA, localB);
	coneC->setLimit(M_PI_2, M_PI_2, 0);
	m_constraints[JOINT_RIGHT_SHOULDER] = coneC;

	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(0, M_PI_2, 0);
	localA.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(0.18), btScalar(0.)));
	localB.getBasis().setEulerZYX(0, M_PI_2, 0);
	localB.setOrigin(m_scale * btVector3(btScalar(0.), btScalar(-0.14), btScalar(0.)));
	hingeC = new btHingeConstraint(*m_rigidBodies[BODYPART_RIGHT_UPPER_ARM], *m_rigidBodies[BODYPART_RIGHT_LOWER_ARM], localA, localB);
	hingeC->setLimit(btScalar(-M_PI_2), btScalar(0));
	m_constraints[JOINT_RIGHT_ELBOW] = hingeC;
}