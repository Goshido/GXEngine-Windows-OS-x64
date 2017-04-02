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
		GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;
};


#endif GX_SPHERE_SHAPE
