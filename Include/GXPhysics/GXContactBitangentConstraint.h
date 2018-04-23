// version 1.0

#ifndef GX_CONTACT_BITANGENT_CONSTRAINT
#define GX_CONTACT_BITANGENT_CONSTRAINT


#include <GXPhysics/GXConstraint.h>
#include <GXPhysics/GXContact.h>


class GXContactBitangentConstraint : public GXConstraint
{
	public:
		GXContactBitangentConstraint ( GXContact &contact, GXUInt linkedContacts );
		~GXContactBitangentConstraint ();
};


#endif // GX_CONTACT_BITANGENT_CONSTRAINT
