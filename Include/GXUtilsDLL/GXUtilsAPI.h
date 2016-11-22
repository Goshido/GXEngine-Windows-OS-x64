#ifndef GX_EDITOR_UTILS_API
#define GX_EDITOR_UTILS_API


#include <GXCommon/GXBKEStructs.h>
#include <GXCommon/GXMTRStructs.h>
#include <GXCommon/GXCFGStruct.h>

typedef GXVoid ( GXCALL* PFNGXEXPORTBKEPROC ) ( const GXWChar* filename, GXBakeInfo &info );
typedef GXVoid ( GXCALL* PFNGXLOADBKEPROC ) ( GXWChar* fileName, GXBakeInfo &out_bake_info );

typedef GXVoid ( GXCALL* PFNGXEXPORTMTRPROC ) ( const GXWChar* fileName, GXMaterialInfo &mat_info );
typedef GXVoid ( GXCALL* PFNGXLOADMTRPROC ) ( GXWChar* fileName, GXMaterialInfo& out_mat_info );

typedef GXVoid ( GXCALL* PFNGXEXPORTCFGPROC ) ( const GXEngineConfiguration &config_info );
typedef GXVoid ( GXCALL* PFNGXLOADCFGPROC ) ( GXEngineConfiguration &out_config_info );

typedef GXBakeInfo ( GXCALL* PFNGXCREATEBAKEINFOPROC ) ();
typedef GXVoid ( GXCALL* PFNGXDESTROYBAKEINFOPROC ) ( GXBakeInfo &info );

typedef GXMaterialInfo ( GXCALL* PFNGXCREATEMATERIALINFOPROC ) ();
typedef GXVoid ( GXCALL* PFNGXDESTROYMATERIALINFOPROC ) ( GXMaterialInfo &info );


#endif	//GX_EDITOR_UTILS_API