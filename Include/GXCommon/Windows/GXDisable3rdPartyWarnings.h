//version 1.0
//No include guard allowed
//Not include this explicitly! Use GXCommon/GXDisable3rdPartyWarnings.h instead.


#include "GXDisableWarning.h"


#pragma warning ( push )


GX_DISABLE_WARNING ( warning ( disable : 4100 ) )	// 'identifier' : unreferenced formal parameter
GX_DISABLE_WARNING ( warning ( disable : 4263 ) )	// 'function' : member function does not override any base class virtual member function
GX_DISABLE_WARNING ( warning ( disable : 4264 ) )	// 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
GX_DISABLE_WARNING ( warning ( disable : 4266 ) )	// no override available for virtual member function from base 'BaseObject'; function is hidden
GX_DISABLE_WARNING ( warning ( disable : 4365 ) )	// 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
GX_DISABLE_WARNING ( warning ( disable : 4464 ) )	// relative include path contains '..'
GX_DISABLE_WARNING ( warning ( disable : 4471 ) )	// 'enumeration': a forward declaration of an unscoped enumeration must have an underlying type (int assumed)
GX_DISABLE_WARNING ( warning ( disable : 4505 ) )	// 'function' : unreferenced local function has been removed
GX_DISABLE_WARNING ( warning ( disable : 4514 ) )	// 'function' : unreferenced inline function has been removed
GX_DISABLE_WARNING ( warning ( disable : 4555 ) )	// expression has no effect; expected expression with side-effect
GX_DISABLE_WARNING ( warning ( disable : 4625 ) )	// copy constructor was implicitly defined as deleted
GX_DISABLE_WARNING ( warning ( disable : 4626 ) )	// assignment operator was implicitly defined as deleted
GX_DISABLE_WARNING ( warning ( disable : 4710 ) )	// 'function' : function not inlined
GX_DISABLE_WARNING ( warning ( disable : 5026 ) )	// move constructor was implicitly defined as deleted
GX_DISABLE_WARNING ( warning ( disable : 5027 ) )	// move assignment operator was implicitly defined as deleted
GX_DISABLE_WARNING ( warning ( disable : 5031 ) )	// #pragma warning(pop): likely mismatch, popping warning state pushed in different file
