//version 1.1

#ifndef GX_DRAWABLE
#define	GX_DRAWABLE


#include <GXCommon/GXTypes.h>

class GXDrawable
{
	public:
		virtual ~GXDrawable () { /*PURE VIRTUAL*/ };
		virtual GXVoid Draw () = 0;
};


#endif //GX_DRAWABLE