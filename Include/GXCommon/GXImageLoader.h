//version 1.1

#ifndef GX_IMAGE_LOADER
#define GX_IMAGE_LOADER


#include "GXTypes.h"

//numChannels	components
//1	alpha
//2	luminance, alpha
//3	red, green, blue
//4	red, green, blue, alpha


GXBool GXCALL GXLoadImage ( const GXWChar* file_name, GXUInt &width, GXUInt &height, GXUByte &numChannels, GXUByte** data );


#endif //GX_IMAGE_LOADER
