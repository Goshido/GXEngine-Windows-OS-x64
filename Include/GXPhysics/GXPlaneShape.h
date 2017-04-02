//version 1.0

#ifndef GX_PLANE_SHAPE
#define GX_PLANE_SHAPE


#include "GXShape.h"


class GXPlaneShape : public GXShape
{
	private:
		GXPlane		plane;

	public:
		GXPlaneShape ( GXRigidBody* body, const GXPlane& plane );
		const GXPlane& GetPlane () const;

		GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
		GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;
};


#endif GX_PLANE_SHAPE
