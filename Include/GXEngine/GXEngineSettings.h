//version 1.0

#ifndef GX_ENGINE_SETTINGS
#define GX_ENGINE_SETTINGS


#include <GXCommon/GXTypes.h>


struct GXEngineSettings
{
	GXUShort		rendererWidth;
	GXUShort		rendererHeight;
	GXUShort		potWidth;
	GXUShort		potHeight;

	GXBool			windowed;
	GXBool			vSync;

	GXUChar			resampling;
	GXUChar			anisotropy;
	
	GXBool			dof;
	GXBool			motionBlur;
};


#endif //GX_ENGINE_SETTINGS