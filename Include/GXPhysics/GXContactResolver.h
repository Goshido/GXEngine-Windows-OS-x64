// version 1.2

#ifndef GX_CONTACT_RESOLVER
#define GX_CONTACT_RESOLVER


#include "GXContact.h"
#include "GXPairBodyConstraintSolver.h"
#include "GXSingleBodyConstraintSolver.h"


class GXContactResolver
{
	private:
		GXPairBodyConstraintSolver		pairBodyConstraintSolver;
		GXSingleBodyConstraintSolver	singleBodyConstraintSolver;

		GXFloat							baumgarteFactor;

		GXFloat							stationaryThreshold;
		GXFloat							squareStationaryThreshold;

	public:
		GXContactResolver ();
		~GXContactResolver ();

		// Baumgarte factor - is optimization factor which has no physics explanation.
		// It is used to improve simulation stability.
		// It is range [0.0f, 1.0f].
		// Erin Catto proposed that you start increasing the value from 0.0f until your
		// physics system starts to become unstable, and then use half that value.
		// See http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/
		GXFloat GetBaumgarteFactor () const;
		GXVoid SetBaumgarteFactor ( GXFloat factor );

		// Stationary threshold is magnitude of relative contact point velocity projection onto tangent / bitangent plane.
		// If this magnitude is less than stationary threshold contact point is considered to be stationary and will be
		// used coefficient of static friction. Otherwise will be used coefficient of dynamic friction.
		GXFloat GetStationaryThreshold () const;
		GXVoid SetStationaryThreshold ( GXFloat threshold );

		GXVoid ResolveContacts ( GXContact* contactArray, GXUInt numContacts );

	private:
		GXVoid ResolveSingleBodyContacts ( GXContact* contacts );
		GXVoid ResolvePairBodyContacts ( GXContact* contacts );

		GXContactResolver ( const GXContactResolver &other ) = delete;
		GXContactResolver& operator = ( const GXContactResolver &other ) = delete;
};


#endif // GX_CONTACT_RESOLVER
