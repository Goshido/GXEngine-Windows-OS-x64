// version 1.2

#ifndef GX_ENGINE_SETTINGS
#define GX_ENGINE_SETTINGS


#include <GXCommon/GXTypes.h>


struct GXEngineSettings
{
    GXUShort    _rendererWidth;
    GXUShort    _rendererHeight;
    GXUShort    _potWidth;
    GXUShort    _potHeight;

    GXBool      _windowed;
    GXBool      _vSync;

    GXUChar     _resampling;
    GXUChar     _anisotropy;
    
    GXBool      _dof;
    GXBool      _motionBlur;
};

extern GXEngineSettings gx_EngineSettings;


#endif // GX_ENGINE_SETTINGS
