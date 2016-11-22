#include <GXUtilsDLL/GXUtilsDLL.h>
#include <GXCommon/GXBKESaver.h>
#include <GXCommon/GXBKELoader.h>
#include <GXCommon/GXMTRSaver.h>
#include <GXCommon/GXMTRLoader.h>
#include <GXCommon/GXCFGSaver.h>
#include <GXCommon/GXCFGLoader.h>


GXDLLEXPORT GXVoid GXCALL GXUtilsExportBKE ( GXWChar* fileName, const GXBakeInfo &info )
{
	GXExportBKE ( fileName, info );
	#ifdef GX_DLL_DEBUG_REPOTRS
		wprintf ( L"GXEditorUtils::DEBUG::GXExportBKE сработала\n" );
	#endif
}

GXDLLEXPORT GXVoid GXCALL GXUtilsLoadBKE ( GXWChar* fileName, GXBakeInfo &out_bake_info )
{
	GXLoadBKE ( fileName, out_bake_info );
	#ifdef GX_DLL_DEBUG_REPOTRS
		wprintf ( L"GXEditorUtils::DEBUG::GXLoadBKE сработала\n" );
	#endif
}

GXDLLEXPORT GXVoid GXCALL GXUtilsExportMTR ( GXWChar* fileName, const GXMaterialInfo &mat_info )
{
	GXExportMTR ( fileName, mat_info );
	#ifdef GX_DLL_DEBUG_REPOTRS
		wprintf ( L"GXEditorUtils::DEBUG::GXExportMTR сработала\n" );
	#endif
}

GXDLLEXPORT GXVoid GXCALL GXUtilsLoadMTR ( GXWChar* fileName, GXMaterialInfo &out_mat_info )
{
	GXLoadMTR ( fileName, out_mat_info );
	#ifdef GX_DLL_DEBUG_REPOTRS
		wprintf ( L"GXEditorUtils::DEBUG::GXLoadMTR сработала\n" );
	#endif
}

GXDLLEXPORT GXVoid GXCALL GXUtilsExportCFG ( const GXEngineConfiguration &config_info  )
{
	GXExportCFG ( config_info );
	#ifdef GX_DLL_DEBUG_REPOTRS
		wprintf ( L"GXEditorUtils::DEBUG::GXExportCFG сработала\n" );
	#endif
}

GXDLLEXPORT GXVoid GXCALL GXUtilsLoadCFG ( GXEngineConfiguration &out_config_info )
{
	GXLoadCFG ( out_config_info );
	#ifdef GX_DLL_DEBUG_REPOTRS
		wprintf ( L"GXEditorUtils::DEBUG::GXLoadCFG сработала\n" );
	#endif
}

GXDLLEXPORT GXBakeInfo GXCALL GXUtilsCreateBakeInfo ()
{
	GXBakeInfo ans;
	return ans;
}

GXDLLEXPORT GXVoid GXCALL GXUtilsDestroyBakeInfo ( GXBakeInfo &info )
{
	info.Cleanup ();
}

GXDLLEXPORT GXMaterialInfo GXCALL GXUtilsCreateMaterialInfo ()
{
	GXMaterialInfo ans;
	return ans;
}

GXDLLEXPORT GXVoid GXCALL GXUtilsDestroyMaterialInfo ( GXMaterialInfo &info )
{
	info.Cleanup ();
}