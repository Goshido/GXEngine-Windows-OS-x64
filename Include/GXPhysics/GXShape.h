//version 1.0

#ifndef GX_SHAPE
#define GX_SHAPE


#include "GXRigidBody.h"


class GXShape
{
	protected:
		GXRigidBody*	body;
		GXMat4			transformRigidBody;
		GXMat4			transformWorld;

	public:
		const GXMat4& GetTransformWorld () const;
		GXVoid CalculateCacheData ();
		GXRigidBody* GetRigidBody () const;
};


#endif GX_SHAPE
