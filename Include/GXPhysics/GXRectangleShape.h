// version 1.0

#ifndef GX_RECTANGLE_SHAPE
#define GX_RECTANGLE_SHAPE


#include "GXShape.h"


class GXRectangleShape : public GXShape
{
	private:
		GXFloat		width;
		GXFloat		depth;

	public:
		explicit GXRectangleShape ( GXRigidBody* body, GXFloat width, GXFloat depth );
		~GXRectangleShape () override;

		GXFloat GetWidth () const;
		GXFloat GetHeight () const;

		GXVoid CalculateInertiaTensor ( GXFloat mass ) override;
		GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const override;

	protected:
		GXVoid UpdateBoundsWorld () override;

	private:
		GXRectangleShape () = delete;
		GXRectangleShape ( const GXRectangleShape &other ) = delete;
		GXRectangleShape& operator = ( const GXRectangleShape &other ) = delete;
};


#endif // GX_RECTANGLE_SHAPE
