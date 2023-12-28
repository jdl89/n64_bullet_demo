#pragma once

// C++.
#include <vector>

// 3rd Party.
#include <bullet/btBulletDynamicsCommon.h> // This is the main Bullet include file, contains most common include files.

class PhysicalAssembly
{
public:
    PhysicalAssembly() = default;
    virtual ~PhysicalAssembly() = default;

	const std::vector<btRigidBody*>& GetRigidBodies() const { return m_rigidBodies; }
	const std::vector<btCollisionObject*>& GetColliders() const { return m_colliders; }
	const std::vector<btTypedConstraint*>& GetConstraints() const { return m_constraints; }
	const std::vector<btMotionState*>& GetMotionStates() const { return m_motionStates; }
	const std::vector<btCollisionShape*>& GetShapes() const { return m_shapes; }
	const std::vector<btActionInterface*>& GetActions() const { return m_actions; }

	void Initialize();

protected:

	virtual void InitializeImpl() {}
	virtual void InitializeShapes() {}
	virtual void InitializeBodies() {}
	virtual void InitializeColliders() {}
	virtual void InitializeConstraints() {}
	virtual void InitializeActions() {}

	std::vector<btRigidBody*> m_rigidBodies;
	std::vector<btCollisionObject*> m_colliders;
	std::vector<btTypedConstraint*> m_constraints;
	std::vector<btMotionState*> m_motionStates;
	std::vector<btCollisionShape*> m_shapes;
	std::vector<btActionInterface*> m_actions;
};