//version 1.0

#include <GXPhysics/GXParticleLink.h>

GXFloat GXParticleLink::GetCurrrentLength () const
{
	GXVec3 relativePosition;
	GXSubVec3Vec3 ( relativePosition, particles[ 0 ]->GetLocation (), particles[ 1 ]->GetLocation () );
	return GXLengthVec3 ( relativePosition );
}
