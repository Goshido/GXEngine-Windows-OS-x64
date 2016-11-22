//version 1.5

#ifndef GX_VAO_STORAGE
#define GX_VAO_STORAGE


#include "GXOpenGL.h"
#include <GXCommon/GXMath.h>


#define GX_VERTEX_INDEX		0
#define GX_UV_COORD_INDEX	1
#define GX_NORMAL_INDEX		2
#define GX_TANGENT_INDEX	3
#define GX_BITANGENT_INDEX	4


struct GXVAOInfo
{
	GXAABB	bounds;

	GLuint	vao;
	GXUInt	numVertices;
	GXUInt	numElements;
};

GXVoid GXCALL GXGetVAOFromOBJ ( GXVAOInfo &out, const GXWChar* fileName, const GXWChar* cacheFileName );
GXVoid GXCALL GXGetVAOFromNativeStaticMesh ( GXVAOInfo &out, const GXWChar* fileName );

GXVoid GXCALL GXRemoveVAO ( const GXVAOInfo &info );
GXUInt GXCALL GXGetTotalVAOStorageObjects ( const GXWChar** lastVAO );


#endif //GX_VAO_STORAGE