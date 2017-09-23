//version 1.1

#ifndef GX_DLL
#define GX_DLL


#include "GXResource.h"
#include "GXUIResource.h"
#include <GXCommon/GXTypes.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <max.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <custcont.h>
#include <modstack.h>
#include <GXCommon/GXRestoreWarnings.h>


#define GXDLLEXPORT extern "C" __declspec ( dllexport )


extern HINSTANCE	gx_Hinstance;

extern Class_ID		gx_UtilityClassID;
extern Class_ID		gx_ModifierClassID;

extern Interface*	gx_UtilityInterface;


GXVoid GXConvertChar2Wchar ( GXWChar** outStr, const GXChar* inStr );


#endif //GX_DLL
