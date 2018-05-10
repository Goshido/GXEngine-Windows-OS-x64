// version 1.2

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include "GXSkeleton.h"
#include "GXSkinningMaterial.h"
#include <GXCommon/GXMath.h>


enum class eGXMeshStreamIndex : GLuint
{
	CurrenVertex		= 0u,
	UV					= 1u,
	Normal				= 2u,
	Tangent				= 3u,
	Bitangent			= 4u,
	SkinningIndices		= 5u,
	SkinningWeights		= 6u,
	LastFrameVertex		= 7u,
	Color				= 8u
};

class GXMesh;
class GXSkin;
class GXMeshGeometry
{
	private:
		GXMesh*					mesh;
		GLuint					meshVAO;
		GLenum					topology;

		GXSkin*					skin;
		GXUByte					skinningSwitchIndex;
		GXSkinningMaterial*		skinningMaterial;

		GXAABB					boundsLocal;

		GLuint					poseVAO[ 2 ];
		GXMesh*					pose[ 2 ];

	public:
		GXMeshGeometry ();
		~GXMeshGeometry ();

		GXVoid Render();

		GXVoid SetBoundsLocal ( const GXAABB& bounds );
		const GXAABB& GetBoundsLocal () const;

		GXVoid SetTotalVertices ( GLsizei totalVertices );
		GXVoid FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage );
		GXVoid SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset );

		GXVoid SetTopology ( GLenum newTopology );

		GXVoid UpdatePose ( const GXSkeleton &skeleton );

		// Unloads previous mesh if it exists.
		GXBool LoadMesh ( const GXWChar* fileName );

		// Unloads previous skin if it exists.
		GXBool LoadSkin ( const GXWChar* fileName );

		static GXUInt GXCALL GetTotalLoadedMeshes ( const GXWChar** lastMesh );
		static GXUInt GXCALL GetTotalLoadedSkins ( const GXWChar** lastSkin );

	private:
		GXVoid UpdateGraphicResources ();

		GXMeshGeometry ( const GXMeshGeometry &other ) = delete;
		GXMeshGeometry& operator = ( const GXMeshGeometry &other ) = delete;
};


#endif // GX_MESH_GEOMETRY
