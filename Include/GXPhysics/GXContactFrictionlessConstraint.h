// version 1.0

#ifndef GX_CONTACT_FRICTIONLESS_CONSTRAINT
#define GX_CONTACT_FRICTIONLESS_CONSTRAINT


#include "GXConstraint.h"
#include "GXContact.h"


class GXContactFrictionlessConstraint : public GXConstraint
{
	public:
		// Baumgarte factor - is optimization factor which has no physics explanation.
		// It is used to improve simulation stability.
		// It is range [0.0f, 1.0f].
		// Erin Catto proposed that you start increasing the value from 0.0f until your
		// physics system starts to become unstable, and then use half that value.
		// See http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/
		explicit GXContactFrictionlessConstraint ( GXContact &contact, GXFloat baumgarteFactor );

		~GXContactFrictionlessConstraint () override;
};


#endif // GX_CONTACT_FRICTIONLESS_CONSTRAINT
