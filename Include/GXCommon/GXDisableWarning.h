// verison 1.0

#ifndef GX_DISABLE_WARNING
#define GX_DISABLE_WARNING


#ifdef _MSC_VER
	#include "Windows/GXDisableWarning.h"
#elif __GNUC__
	#include "Posix/GXDisableWarning.h"
#else
	#error Unsupported compiler detected (GXDisableWarning.h)!
#endif


#endif // GX_DISABLE_WARNING
