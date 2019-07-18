// version 1.0

#ifndef GX_CRASH_REPORT
#define GX_CRASH_REPORT


#ifdef _MSC_VER

#include "Windows/GXCrashReport.h"

#elif __GNUC__

#include "Posix/GXCrashReport.h"

#else

#error Unsupported compiler detected!

#endif // __GNU__


#endif // GX_CRASH_REPORT
