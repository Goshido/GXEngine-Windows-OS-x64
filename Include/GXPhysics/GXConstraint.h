//version 1.0

#ifndef GX_CONSTRAINT
#define GX_CONSTRAINT


#include "GXSparseMatrix.h"
#include <GXCommon/GXMath.h>


class GXConstraint
{
	protected:
		GXSparseMatrixElement		jacobian[ 2 ];
		GXFloat						bias;
		GXVec2						lambdaRange;

	public:
		const GXSparseMatrixElement* GetJacobian () const;
		GXFloat GetBias () const;
		const GXVec2& GetLambdaRange () const;

	protected:
		GXConstraint ();
		virtual ~GXConstraint ();
};


#endif //GX_CONSTRAINT
