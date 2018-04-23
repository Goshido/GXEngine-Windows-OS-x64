// version 1.2

#ifndef GX_ENGINE_API
#define GX_ENGINE_API


#include "GXPhysXAdapter.h"

typedef GXPhysXAdapter* ( GXCALL* PFNGXPHYSXCREATE ) ();
typedef GXVoid ( GXCALL* PFNGXPHYSXDESTROY ) ( GXPhysXAdapter* physics );

//--------------------------------------------------------------------------------------------------

#include "GXOGGVorbis.h"

typedef GXVoid ( GXCALL* PFNGXOGGVORBISINIT ) ( GXOGGVorbisFunctions &out );

//--------------------------------------------------------------------------------------------------

#include "GXOpenAL.h"

typedef GXBool ( GXCALL* PFNGXOPENALINIT ) ( GXOpenALFunctions &out );

//--------------------------------------------------------------------------------------------------

#include "GXFreeType.h"

typedef GXBool ( GXCALL* PFNGXFREETYPEINIT ) ( GXFreeTypeFunctions &out, FT_Library &library );

//--------------------------------------------------------------------------------------------------

#include "GXXInput.h"

typedef GXVoid ( GXCALL* PFNGXXINPUTINITPROC ) ( GXXInputFunctions &out );

//--------------------------------------------------------------------------------------------------


#endif // GX_ENGINE_API
