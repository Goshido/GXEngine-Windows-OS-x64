//version 1.2

#ifndef GX_CAMERAMAN
#define GX_CAMERAMAN


#include <GXCommon/GXMath.h>
#include "GXCamera.h"

class GXCameraman
{
	protected:
		GXCamera* camera;

	public:
		GXCameraman ( GXCamera* camera );
		virtual ~GXCameraman ();

		GXVoid SetCamera ( GXCamera* camera );

		virtual GXVoid Action ( const GXVec3 &stare_location, const GXQuat &stare_rotation ) = 0;
		virtual GXVoid Action ( const GXVec3 &stare_location, const GXMat4 &stare_rotation ) = 0;
};


#endif	//GX_CAMERAMAN