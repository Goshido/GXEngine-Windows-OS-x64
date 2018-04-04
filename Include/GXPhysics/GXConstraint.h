//version 1.0

#ifndef GX_CONSTRAINT
#define GX_CONSTRAINT


#include "GXSparseMatrix.h"


class GXConstraint
{
	protected:
		GXSparseMatrixElement		jacobian[ 2 ];
		GXFloat						bias;
		GXFloat						minimumLambda;
		GXFloat						maximumLambda;

	public:
		const GXSparseMatrixElement* GetJacobian () const;
		GXFloat GetBias () const;
		GXVoid GetLambdaRange ( GXFloat &minimum, GXFloat &maximum ) const;

	protected:
		GXConstraint ();
		virtual ~GXConstraint ();
};


#endif //GX_CONSTRAINT
