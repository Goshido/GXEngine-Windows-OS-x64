//version 1.0

#ifndef GX_SPHERE_SHAPE
#define GX_SPHERE_SHAPE


#include "GXShape.h"


class GXSphereShape : public GXShape
{
	private:
		GXFloat		radius;

	public:
		GXFloat GetRadius () const;
};


#endif GX_SPHERE_SHAPE
