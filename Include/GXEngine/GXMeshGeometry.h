//version 1.0

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include <GXCommon/GXMath.h>


#define VERTEX_STREAM		0
#define UV_COORD_STREAM		1
#define NORMAL_STREAM		2
#define TANGENT_STREAM		3
#define BITANGENT_STREAM	4

enum class eGXMeshStreamIndex : GLuint
{
	Vertex		= 0,
	UV			= 1,
	Normal		= 2,
	Tangent		= 3,
	Bitangent	= 4	
};


class GXMeshGeometry
{
	private:
		GLsizei		totalElements;
		GLuint		vao;
		GLuint		vbo;

		GXAABB		boundsLocal;
		GXAABB		boundsWorld;

	public:
		GXMeshGeometry ();
		virtual ~GXMeshGeometry ();

		GXVoid Render() const;

		GXVoid SetBoundsLocal ( const GXAABB& bounds );
		const GXAABB& GetBoundsLocal () const;

		GXVoid UpdateBoundsWorld ( const GXMat4& transform );
		const GXAABB& GetBoundsWorld () const;

		GXVoid SetTotalElements ( GLsizei elements );
		GXVoid FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage );
		GXVoid SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset );

		static GXMeshGeometry& GXCALL LoadFromObj ( const GXWChar* fileName );
		static GXVoid GXCALL Remove ( GXMeshGeometry& mesh );

		GXBool operator == ( const GXMeshGeomentryEntry &entry );
};


#endif //GX_MESH_GEOMETRY
