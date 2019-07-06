// version 1.2

#ifndef GX_CFG_STRUCT
#define GX_CFG_STRUCT


#include "GXTypes.h"


struct GXEngineConfiguration final
{
    GXUShort        usRendererWidthResoluton;
    GXUShort        usRendererHeightResoluton;
    GXUChar         chResampling;
    GXUChar         chAnisotropy;
    GXBool          bIsWindowedMode;
    GXBool          bVSync;
    GXBool          bDoF;
    GXBool          bMotionBlur;
};


#endif // GX_CFG_STRUCT
