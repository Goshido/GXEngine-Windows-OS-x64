//version 1.0

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include "GXSkeleton.h"
#include <GXCommon/GXMath.h>


enum class eGXMeshStreamIndex : GLuint
{
	CurrenVertex		= 0,
	UV					= 1,
	Normal				= 2,
	Tangent				= 3,
	Bitangent			= 4,
	SkinningIndices		= 5,
	SkinnngWeights		= 6,
	LastFrameVertex		= 7
};

class GXMeshGeometryEntry;
class GXMeshGeometry
{
	private:
		GLsizei					totalVertices;

		GLuint					staticVAO;
		GLuint					staticVBO;

		GLuint					skeletalVAO[ 2 ];
		GLuint					skinningVAO[ 2 ];
		GLuint					skinningVBO[ 2 ];
		GLuint					skeletalVBO;

		GXUByte					skinningSwitchIndex;

		GLenum					topology;

		GXAABB					boundsLocal;

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

		GXVoid UpdatePose ( const GXSkeleton &skeleton );

		static GXMeshGeometry& GXCALL LoadFromObj ( const GXWChar* fileName );
		static GXMeshGeometry& GXCALL LoadFromStm ( const GXWChar* fileName );
		static GXMeshGeometry& GXCALL LoadFromSkm ( const GXWChar* fileName );

		static GXVoid GXCALL RemoveMeshGeometry ( GXMeshGeometry& mesh );
		static GXUInt GXCALL GetTotalLoadedMeshGeometries ( const GXWChar** lastMeshGeometry );

		GXBool operator == ( const GXMeshGeometryEntry &entry ) const;
		GXVoid operator = ( const GXMeshGeometry &meshGeometry );

	private:
		GXVoid InitStaticResources ();
		GXVoid InitSkeletalResources ();

		static GXMeshGeometry& GXCALL GetGeometryFromStm ( const GXWChar* fileName );
		GXBool IsSkeletalMesh () const;
};


#endif //GX_MESH_GEOMETRY
