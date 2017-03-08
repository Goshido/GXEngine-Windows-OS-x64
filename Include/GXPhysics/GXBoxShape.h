//version 1.0

#ifndef GX_BOX_SHAPE
#define GX_BOX_SHAPE


#include "GXShape.h"


struct GXBoxShapeVertices
{
	GXVec3	vertices[ 8 ];
};

class GXBoxShape : public GXShape
{
	private:
		GXFloat width;
		GXFloat height;
		GXFloat depth;

	public:
		GXBoxShape ( GXRigidBody* body, GXFloat width, GXFloat height, GXFloat depth );

		GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
		GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;

		GXFloat GetWidth () const;
		GXFloat GetHeight () const;
		GXFloat GetDepth () const;

		GXVoid GetVecticesWorld ( GXBoxShapeVertices &vertices ) const;
};


#endif GX_BOX_SHAPE
