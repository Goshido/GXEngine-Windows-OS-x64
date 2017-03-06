//version 1.0

#ifndef GX_PLANE_SHAPE
#define GX_PLANE_SHAPE


#include "GXShape.h"


class GXPlaneShape : public GXShape
{
	private:
		GXPlane		plane;

	public:
		const GXPlane& GetPlane () const;
};


#endif GX_PLANE_SHAPE
