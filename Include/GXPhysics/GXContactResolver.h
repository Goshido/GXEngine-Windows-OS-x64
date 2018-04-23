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

		GXVoid ResolveContacts ( GXContact* contactArray, GXUInt numContacts );

	private:
		GXVoid ResolveSingleBodyContacts ( GXContact* contacts );
		GXVoid ResolvePairBodyContacts ( GXContact* contacts );
};


#endif // GX_CONTACT_RESOLVER
