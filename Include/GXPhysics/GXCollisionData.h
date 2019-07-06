// version 1.3

#ifndef GX_COLLISION_DATA
#define GX_COLLISION_DATA


#include "GXContact.h"


class GXCollisionData final
{
    private:
        GXUInt          _maxContacts;

        GXContact*      _contactArray;
        GXContact*      _contacts;
        GXUInt          _contactsLeft;
        GXUInt          _contactCount;

        GXFloat         _friction;
        GXFloat         _restitution;
        GXFloat         _tolerance;

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
