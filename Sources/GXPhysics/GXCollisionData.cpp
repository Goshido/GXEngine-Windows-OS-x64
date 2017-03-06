//version 1.0

#include <GXPhysics/GXCollisionData.h>


GXBool GXCollisionData::HasMoreContacts () const
{
	return contactsLeft > 0;
}

GXVoid GXCollisionData::Reset ( GXUInt maxContacts )
{
	contactsLeft = maxContacts;
	contactCount = 0;
	contacts = contactArray;
}

GXVoid GXCollisionData::AddContacts ( GXUInt count )
{
	contactsLeft -= count;
	contactCount += count;
	contacts += count;
}

GXContact* GXCollisionData::GetContactsBegin () const
{
	return contacts;
}

const GXContact* GXCollisionData::GetAllContacts () const
{
	return contactArray;
}

GXUInt GXCollisionData::GetTotalContacts () const
{
	return contactCount;
}

GXUInt GXCollisionData::GetContactsLeft () const
{
	return contactsLeft;
}

GXFloat GXCollisionData::GetFriction () const
{
	return friction;
}

GXFloat GXCollisionData::GetRestitution () const
{
	return restitution;
}

GXFloat GXCollisionData::GetTolerance () const
{
	return tolerance;
}
