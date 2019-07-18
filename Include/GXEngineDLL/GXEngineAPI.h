// version 1.3

#ifndef GX_ENGINE_API
#define GX_ENGINE_API


#include "GXPhysXAdapter.h"

typedef GXPhysXAdapter* ( GXCALL* GXPhysXCreateFunc ) ();
typedef GXVoid ( GXCALL* GXPhysXDestroyFunc ) ( GXPhysXAdapter* physics );

//--------------------------------------------------------------------------------------------------

#include "GXOGGVorbis.h"

typedef GXVoid ( GXCALL* GXOGGVorbisInitFunc ) ( GXOGGVorbisFunctions &out );

//--------------------------------------------------------------------------------------------------

#include "GXOpenAL.h"

typedef GXBool ( GXCALL* GXOpenALInitFunc ) ( GXOpenALFunctions &out );

//--------------------------------------------------------------------------------------------------

#include "GXFreeType.h"

typedef GXBool ( GXCALL* GXFreeTypeInitFunc ) ( GXFreeTypeFunctions &out, FT_Library &library );

//--------------------------------------------------------------------------------------------------

#include "GXXInput.h"

typedef GXVoid ( GXCALL* GXXInputInitFunc ) ( GXXInputFunctions &out );

//--------------------------------------------------------------------------------------------------


#endif // GX_ENGINE_API
