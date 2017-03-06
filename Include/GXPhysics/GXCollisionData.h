//version 1.0

#ifndef GX_COLLISION_DATA
#define GX_COLLISION_DATA


#include "GXContact.h"


class GXCollisionData
{
	private:
		GXContact*		contactArray;

		GXContact*		contacts;
		GXUInt			contactsLeft;
		GXUInt			contactCount;

		GXFloat			friction;
		GXFloat			restitution;
		GXFloat			tolerance;

	public:
		GXBool HasMoreContacts () const;
		GXVoid Reset ( GXUInt maxContacts );
		GXVoid AddContacts ( GXUInt count );

		GXContact* GetContactsBegin () const;
		const GXContact* GetAllContacts () const;
		GXUInt GetTotalContacts () const;
		GXUInt GetContactsLeft () const;

		GXFloat GetFriction () const;
		GXFloat GetRestitution () const;
		GXFloat GetTolerance () const;
};


#endif GX_COLLISION_DATA
