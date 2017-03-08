//version 1.0

#ifndef GX_CONTACT_GENERATOR
#define GX_CONTACT_GENERATOR


#include "GXCollisionData.h"


class GXContactGenerator
{
	public:
		virtual GXUInt AddContact ( GXCollisionData& collisionData ) const = 0;
};


#endif GX_CONTACT_GENERATOR
