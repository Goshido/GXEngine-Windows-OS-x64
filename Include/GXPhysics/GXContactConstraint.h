//version 1.0

#ifndef GX_CONTACT_CONSTRAINT
#define GX_CONTACT_CONSTRAINT


#include "GXConstraint.h"
#include "GXContact.h"


class GXContactConstraint : public GXConstraint
{
	public:
		explicit GXContactConstraint ( GXContact &contact, GXFloat baumgarteFactor );
		~GXContactConstraint () override;
};


#endif //GX_CONTACT_CONSTRAINT
