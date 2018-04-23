// version 1.0

#ifndef GX_CONTACT_TANGENT_CONSTRAINT
#define GX_CONTACT_TANGENT_CONSTRAINT


#include <GXPhysics/GXConstraint.h>
#include <GXPhysics/GXContact.h>


class GXContactTangentConstraint : public GXConstraint
{
	public:
		GXContactTangentConstraint ( GXContact &contact, GXUInt linkedContacts );
		~GXContactTangentConstraint ();
};


#endif // GX_CONTACT_TANGENT_CONSTRAINT
