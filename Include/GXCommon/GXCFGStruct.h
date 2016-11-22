//version 1.0

#ifndef GX_CFG_STRUCT
#define GX_CFG_STRUCT


#include "GXCommon.h"

struct GXEngineConfiguration
{
	GXUShort		usRendererWidthResoluton;
	GXUShort		usRendererHeightResoluton;
	GXUChar			chResampling;
	GXUChar			chAnisotropy;
	GXBool			bIsWindowedMode;
	GXBool			bVSync;
	GXBool			bDoF;
	GXBool			bMotionBlur;
};


#endif	//GX_CFG_STRUCT