//version 1.0

#ifndef GX_MUTEX
#define GX_MUTEX


#ifdef __GNUC__
	#include "Posix/GXMutex.h"
#else
	#include "Windows/GXMutex.h"
#endif //__GNU__


#endif //GX_MUTEX
