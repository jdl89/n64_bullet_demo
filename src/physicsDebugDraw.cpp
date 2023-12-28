// Infra.
#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>

// Local.
#include "physicsDebugDraw.hpp"

#define kContactPointScaler 0.1f

//////////////////////////////////////////////////////////////////////////
void PhysicsDebugDraw::drawLine( const btVector3& from,const btVector3& to,const btVector3& color )
{
	glColor3f(color[0], color[1], color[2]);
	glVertex3f(from[0], from[1], from[2]);

	glColor3f(color[0], color[1], color[2]);
	glVertex3f(to[0], to[1], to[2]);
}

//////////////////////////////////////////////////////////////////////////
void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color )
{
	// @TODO: Implement the contact point rendering
	/*
	ddVec3_In pointOnBPosition = { PointOnB.x(), PointOnB.y(), PointOnB.z() };
	ddVec3_In normalOnBConverted = { normalOnB.x(), normalOnB.y(), normalOnB.z() };
	ddVec3_In pointColor = { color.x(), color.y(), color.z() };
	dd::point(pointOnBPosition, pointColor);

	auto temp = PointOnB + (normalOnB * distance * 10.0f);

	ddVec3_In to = { temp.x(), temp.y(), temp.z()};

	// Draw the contact normal vector
	dd::arrow(pointOnBPosition, to, pointColor, kContactPointScaler);
	*/
}

/*
// @TODO: Attempt to connect the physics engine settings to a MicroGui tool overlay.
void PhysicsDebugDraw::DrawToolOverlay()
{
	int debugDrawFlags = getDebugMode();
	bool altered = false;
	altered |= ImGui::CheckboxFlags("DBG_DrawFrames", &debugDrawFlags, DBG_DrawFrames);
	altered |= ImGui::CheckboxFlags("DBG_DrawWireframe", &debugDrawFlags, DBG_DrawWireframe); ImGui::SameLine();
	altered |= ImGui::CheckboxFlags("DBG_DrawAabb", &debugDrawFlags, DBG_DrawAabb); ImGui::SameLine();
	altered |= ImGui::CheckboxFlags("DBG_DrawContactPoints", &debugDrawFlags, DBG_DrawContactPoints);

	altered |= ImGui::CheckboxFlags("DBG_DrawConstraints", &debugDrawFlags, DBG_DrawConstraints); ImGui::SameLine();
	altered |= ImGui::CheckboxFlags("DBG_DrawConstraintLimits", &debugDrawFlags, DBG_DrawConstraintLimits); ImGui::SameLine();

	altered |= ImGui::CheckboxFlags("DBG_DrawNormals", &debugDrawFlags, DBG_DrawNormals);

	altered |= ImGui::CheckboxFlags("DBG_NoDeactivation", &debugDrawFlags, DBG_NoDeactivation);


	// TODO: Should these be included?
	// altered |= ImGui::CheckboxFlags("DBG_DrawFeaturesText", &debugDrawFlags, DBG_DrawFeaturesText); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_EnableCCD", &debugDrawFlags, DBG_EnableCCD); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_FastWireframe", &debugDrawFlags, DBG_FastWireframe); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_NoHelpText", &debugDrawFlags, DBG_NoHelpText); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_DrawText", &debugDrawFlags, DBG_DrawText); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_ProfileTimings", &debugDrawFlags, DBG_ProfileTimings); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_EnableSatComparison", &debugDrawFlags, DBG_EnableSatComparison); ImGui::SameLine();
	// altered |= ImGui::CheckboxFlags("DBG_DisableBulletLCP", &debugDrawFlags, DBG_DisableBulletLCP); ImGui::SameLine();

	if (altered)
	{
		setDebugMode(debugDrawFlags);
	}
}
*/

//////////////////////////////////////////////////////////////////////////
void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
	// @TODO: Implement this.
	//throw std::exception("The method or operation is not implemented.");
}

//////////////////////////////////////////////////////////////////////////
void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
	// @TODO: Implement this.
	//throw std::exception("The method or operation is not implemented.");
}