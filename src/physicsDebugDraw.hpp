#pragma once

#include <LinearMath/btIDebugDraw.h>

//////////////////////////////////////////////////////////////////////////
class PhysicsDebugDraw : public btIDebugDraw
{
public:
	PhysicsDebugDraw() : mDebugMode(DBG_DrawWireframe|DBG_DrawContactPoints|DBG_DrawFrames) {}
	virtual ~PhysicsDebugDraw() {}

	virtual void drawLine( const btVector3& from,const btVector3& to,const btVector3& color );
	virtual void drawContactPoint( const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color );
	virtual void reportErrorWarning( const char* warningString );
	virtual void draw3dText( const btVector3& location,const char* textString );
	virtual void setDebugMode(int debugMode ) { mDebugMode = debugMode; }
	virtual int getDebugMode() const { return mDebugMode; }

	void DrawToolOverlay();

	int& GetDebugModeConfig() { return mDebugMode; }

private:
	int mDebugMode;
};