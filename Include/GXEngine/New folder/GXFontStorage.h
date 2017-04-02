//version 1.5

#ifndef GX_FONT_STORAGE
#define GX_FONT_STORAGE


#include "GXFont.h"


GXFont* GXCALL GXGetFont ( const GXWChar* fileName, GXUShort size );
GXVoid GXCALL GXRemoveFont ( const GXFont* font );
GXUInt GXCALL GXGetTotalFontStorageObjects ( const GXWChar** lastFont );


#endif //GX_FONT_STORAGE
