//version 1.0

#ifndef GX_SPHERE_SHAPE
#define GX_SPHERE_SHAPE


#include "GXShape.h"


class GXSphereShape : public GXShape
{
	private:
		GXFloat		radius;

	public:
		GXSphereShape ( GXRigidBody* body, GXFloat radius );
		GXFloat GetRadius () const;
		GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
};


#endif GX_SPHERE_SHAPE
