//version 1.0
//No include guard allowed
//Not include this explicitly! Use GXCommon/GXDisable3rdPartyWarnings.h instead.


#include "GXDisableWarning.h"


#pragma warning ( push )

GX_DISABLE_WARNING ( warning ( disable : 4100 ) )	// 'identifier' : unreferenced formal parameter
GX_DISABLE_WARNING ( warning ( disable : 4365 ) )	// 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
GX_DISABLE_WARNING ( warning ( disable : 4505 ) )	// 'function' : unreferenced local function has been removed
GX_DISABLE_WARNING ( warning ( disable : 4514 ) )	// 'function' : unreferenced inline function has been removed
GX_DISABLE_WARNING ( warning ( disable : 4555 ) )	// expression has no effect; expected expression with side-effect
GX_DISABLE_WARNING ( warning ( disable : 4710 ) )	// 'function' : function not inlined
GX_DISABLE_WARNING ( warning ( disable : 5031 ) )	// #pragma warning(pop): likely mismatch, popping warning state pushed in different file
