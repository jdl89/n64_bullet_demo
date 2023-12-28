// Local.
#include "PhysicalAssembly.hpp"

void PhysicalAssembly::Initialize()
{
	InitializeImpl();

	InitializeShapes();
	InitializeColliders();
	InitializeBodies();
	InitializeConstraints();
	InitializeActions();
}