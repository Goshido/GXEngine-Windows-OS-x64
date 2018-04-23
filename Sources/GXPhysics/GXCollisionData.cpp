// version 1.0

#include <GXPhysics/GXCollisionData.h>
#include <GXCommon/GXMemory.h>


GXCollisionData::GXCollisionData ( GXUInt maxContacts )
{
	this->maxContacts = maxContacts;
	contactArray = (GXContact*)malloc ( maxContacts * sizeof ( GXContact ) );
	Reset ();
}

GXCollisionData::~GXCollisionData ()
{
	free ( contactArray );
}

GXBool GXCollisionData::HasMoreContacts () const
{
	return contactsLeft > 0;
}

GXVoid GXCollisionData::Reset ()
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

GXContact* GXCollisionData::GetAllContacts () const
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
