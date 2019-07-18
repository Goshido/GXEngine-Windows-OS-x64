// version 1.0

#ifndef GX_CRASH_REPORT_WINDOWS
#define GX_CRASH_REPORT_WINDOWS


#include <GXCommon/GXTypes.h>


class GXCrashReport final
{
    public:
        GXCrashReport ();
        ~GXCrashReport ();

    private:
        static LONG CALLBACK UnhandledExceptionFilter ( EXCEPTION_POINTERS* exceptionPointers );

        GXCrashReport ( const GXCrashReport &other ) = delete;
        GXCrashReport& operator = ( const GXCrashReport &other ) = delete;
};


#endif // GX_CRASH_REPORT_WINDOWS
