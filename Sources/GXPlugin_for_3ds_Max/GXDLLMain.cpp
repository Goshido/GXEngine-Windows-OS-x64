//version 1.1


#include <GXPlugin_for_3ds_Max/GXDLL.h>
#include <GXPlugin_for_3ds_Max/GXUtility.h>


#define GX_PLUGIN_TOTAL_LIB_CLASSES		1

HINSTANCE			gx_Hinstance;
static GXBool		gx_NeedInitDLL = GX_TRUE;

BOOL WINAPI DllMain ( HINSTANCE hinst, DWORD /*fdwReason*/, LPVOID /*reserved*/ )
{
	gx_Hinstance = hinst;

	if ( gx_NeedInitDLL )
	{
		gx_NeedInitDLL = GX_FALSE;
		InitCommonControls ();
	}

	return GX_TRUE;
}

static GXChar gx_LibDescription[] = "GXPlugin\0";

GXDLLEXPORT const GXChar* LibDescription ()
{
	return gx_LibDescription;
}

GXDLLEXPORT GXInt LibNumberClasses ()
{
	return GX_PLUGIN_TOTAL_LIB_CLASSES;
}

GXDLLEXPORT ClassDesc* LibClassDesc ( GXInt i )
{
	switch ( i )
	{
		case 0:
			return GXGetUtilityDesc ();

		default:
			return nullptr;
	}
}

GXDLLEXPORT GXULong LibVersion ()
{
	return VERSION_3DSMAX;
}
