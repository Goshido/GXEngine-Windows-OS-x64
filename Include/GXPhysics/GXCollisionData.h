// version 1.0

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
		GXUInt			maxContacts;

		GXFloat			friction;
		GXFloat			restitution;
		GXFloat			tolerance;

	public:
		GXCollisionData ( GXUInt maxContacts );
		~GXCollisionData ();

		GXBool HasMoreContacts () const;
		GXVoid Reset ();
		GXVoid AddContacts ( GXUInt count );

		GXContact* GetContactsBegin () const;
		GXContact* GetAllContacts () const;
		GXUInt GetTotalContacts () const;
		GXUInt GetContactsLeft () const;

		GXFloat GetFriction () const;
		GXFloat GetRestitution () const;
		GXFloat GetTolerance () const;
};


#endif // GX_COLLISION_DATA
