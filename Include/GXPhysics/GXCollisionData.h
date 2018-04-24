// version 1.1

#ifndef GX_COLLISION_DATA
#define GX_COLLISION_DATA


#include "GXContact.h"


class GXCollisionData
{
	private:
		GXUInt			maxContacts;

		GXContact*		contactArray;
		GXContact*		contacts;
		GXUInt			contactsLeft;
		GXUInt			contactCount;

		GXFloat			friction;
		GXFloat			restitution;
		GXFloat			tolerance;

	public:
		explicit GXCollisionData ( GXUInt maxContacts );
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

	private:
		GXCollisionData ( const GXCollisionData &other ) = delete;
		GXCollisionData& operator = ( const GXCollisionData &other ) = delete;
};


#endif // GX_COLLISION_DATA
