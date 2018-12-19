// version 1.3

#include <GXCommon/GXTime.h>

GX_DISABLE_COMMON_WARNINGS

#include <Windows.h>
#include <intrin.h>

GX_RESTORE_WARNING_STATE


GXDouble GXCALL GXGetProcessorTicks ()
{
    return static_cast<GXDouble> ( __rdtsc () );
}

GXDouble GXCALL GXGetProcessorTicksPerSecond ()
{
    static GXDouble ticks = -1.0;

    if ( ticks <= 0.0 )
    {
        HKEY hKey;
        DWORD procSpeed;
        DWORD buflen;
        DWORD ret;

        if ( !RegOpenKeyExW ( HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey ) )
        {
            procSpeed = 0;
            buflen = sizeof ( procSpeed );

            ret = static_cast<DWORD> ( RegQueryValueExW ( hKey, L"~MHz", 0, 0, (LPBYTE)&procSpeed, &buflen ) );

            if ( ret != ERROR_SUCCESS )
                ret = static_cast<DWORD> ( RegQueryValueExW ( hKey, L"~Mhz", 0, 0, (LPBYTE)&procSpeed, &buflen ) );

            if ( ret != ERROR_SUCCESS )
                ret = static_cast<DWORD> ( RegQueryValueExW ( hKey, L"~mhz", 0, 0, (LPBYTE)&procSpeed, &buflen ) );

            RegCloseKey ( hKey );

            if ( ret == ERROR_SUCCESS )
            {
                ticks = static_cast<GXDouble> ( procSpeed * 1000000.0 );
            }
        }
    }

    return ticks;
}
