// version 1.3

#include <GXPhysics/GXCollisionData.h>
#include <GXCommon/GXMemory.h>


GXCollisionData::GXCollisionData ( GXUInt maxContacts ):
    _maxContacts ( maxContacts )
{
    _contactArray = static_cast<GXContact*> ( malloc ( maxContacts * sizeof ( GXContact ) ) );
    Reset ();
}

GXCollisionData::~GXCollisionData ()
{
    free ( _contactArray );
}

GXBool GXCollisionData::HasMoreContacts () const
{
    return _contactsLeft > 0;
}

GXVoid GXCollisionData::Reset ()
{
    _contactsLeft = _maxContacts;
    _contactCount = 0;
    _contacts = _contactArray;
}

GXVoid GXCollisionData::AddContacts ( GXUInt count )
{
    _contactsLeft -= count;
    _contactCount += count;
    _contacts += count;
}

GXContact* GXCollisionData::GetContactsBegin () const
{
    return _contacts;
}

GXContact* GXCollisionData::GetAllContacts () const
{
    return _contactArray;
}

GXUInt GXCollisionData::GetTotalContacts () const
{
    return _contactCount;
}

GXUInt GXCollisionData::GetContactsLeft () const
{
    return _contactsLeft;
}

GXFloat GXCollisionData::GetFriction () const
{
    return _friction;
}

GXFloat GXCollisionData::GetRestitution () const
{
    return _restitution;
}

GXFloat GXCollisionData::GetTolerance () const
{
    return _tolerance;
}
