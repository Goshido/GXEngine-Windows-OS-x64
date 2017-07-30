//version 1.3

#ifndef GX_IMAGE_LOADER
#define GX_IMAGE_LOADER


#include "GXTypes.h"

//numChannels	components
//1	alpha
//2	luminance, alpha
//3	red, green, blue
//4	red, green, blue, alpha


GXBool GXCALL GXLoadLDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXUByte** data );
GXBool GXCALL GXLoadHDRImage ( const GXWChar* fileName, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXFloat** data );


#endif //GX_IMAGE_LOADER
