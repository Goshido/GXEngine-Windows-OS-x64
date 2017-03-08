//version 1.0

#ifndef GX_PHYSICS_ENGINE
#define GX_PHYSICS_ENGINE


#include "GXWorld.h"


class GXPhysicsEngine
{
	private:
		static GXPhysicsEngine*	instance;
		GXVec3					gravity;
		GXFloat					sleepEpsilon;
		GXWorld					world;

	public:
		~GXPhysicsEngine ();
		static GXPhysicsEngine* GetInstance ();

		GXVoid SetGravity ( GXFloat x, GXFloat y, GXFloat z );
		const GXVec3& GetGravity () const;

		GXVoid SetSleepEpsilon ( GXFloat epsilon );
		GXFloat GetSleepEpsilon () const;

		GXWorld& GetWorld ();

		GXVoid RunSimulateLoop ( GXFloat deltaTime );

	private:
		GXPhysicsEngine ();
};


#endif //GX_PHYSICS_ENGINE
