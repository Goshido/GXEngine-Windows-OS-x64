//version 1.0

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include <GXCommon/GXMath.h>


enum class eGXMeshStreamIndex : GLuint
{
	Vertex		= 0,
	UV			= 1,
	Normal		= 2,
	Tangent		= 3,
	Bitangent	= 4
};

class GXMeshGeometryEntry;
class GXMeshGeometry
{
	private:
		GLsizei					totalVertices;
		GLuint					vao;
		GLuint					vbo;
		GLenum					topology;

		GXAABB					boundsLocal;

		static GXMeshGeometry	nullObject;

	public:
		GXMeshGeometry ();
		~GXMeshGeometry ();

		GXVoid Render() const;

		GXVoid SetBoundsLocal ( const GXAABB& bounds );
		const GXAABB& GetBoundsLocal () const;

		GXVoid SetTotalVertices ( GLsizei totalVertices );
		GXVoid FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage );
		GXVoid SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset );
		GXVoid SetTopology ( GLenum topology );

		static GXMeshGeometry& GXCALL LoadFromObj ( const GXWChar* fileName );
		static GXMeshGeometry& GXCALL LoadFromStm ( const GXWChar* fileName );

		static GXVoid GXCALL RemoveMeshGeometry ( GXMeshGeometry& mesh );
		static GXUInt GXCALL GetTotalLoadedMeshGeometries ( const GXWChar** lastMeshGeometry );

		GXBool operator == ( const GXMeshGeometryEntry &entry ) const;
		GXVoid operator = ( const GXMeshGeometry &meshGeometry );

	private:
		GXVoid Init ();
		static GXMeshGeometry& GXCALL GetGeometryFromStm ( const GXWChar* fileName );
};


#endif //GX_MESH_GEOMETRY
