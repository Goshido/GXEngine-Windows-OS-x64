// version 1.1

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

	private:
		GXConstraint ( const GXConstraint &other ) = delete;
		GXConstraint& operator = ( const GXConstraint &other ) = delete;
};


#endif // GX_CONSTRAINT
