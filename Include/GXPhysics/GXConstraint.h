// version 1.0

#ifndef GX_CONSTRAINT
#define GX_CONSTRAINT


#include <GXCommon/GXMath.h>


class GXConstraint
{
	protected:
		GXVec6		jacobian[ 2 ];
		GXFloat		bias;
		GXVec2		lambdaRange;

	public:
		const GXVec6* GetJacobian () const;
		GXFloat GetBias () const;
		const GXVec2& GetLambdaRange () const;

	protected:
		GXConstraint ();
		virtual ~GXConstraint ();
};


#endif // GX_CONSTRAINT
