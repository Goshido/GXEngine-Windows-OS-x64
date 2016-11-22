//version 1.3

#ifndef GX_PHYSICS
#define GX_PHYSICS


#include <GXEngineDLL/GXEngineAPI.h>


extern PFNGXPHYSICSCREATE	GXPhysicsCreate;
extern PFNGXPHYSICSDESTROY	GXPhysicsDestroy;

GXBool GXCALL GXPhysXInit ();
GXBool GXCALL GXPhysXDestroy ();


#endif	//GX_PHYSICS