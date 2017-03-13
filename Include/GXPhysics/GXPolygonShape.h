//version 1.0

#ifndef GX_POLYGON_SHAPE
#define GX_POLYGON_SHAPE


#include "GXShape.h"


class GXPolygonShape : public GXShape
{
	private:
		GXFloat		width;
		GXFloat		depth;

	public:
		GXPolygonShape ( GXRigidBody* body, GXFloat widht, GXFloat depth );

		GXFloat GetWidth () const;
		GXFloat GetHeight () const;

		GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
		GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;
};


#endif //GX_POLYGON_SHAPE
