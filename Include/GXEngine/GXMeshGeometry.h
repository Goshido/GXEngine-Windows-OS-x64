// version 1.2

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include "GXSkeleton.h"
#include "GXSkinningMaterial.h"
#include <GXCommon/GXMath.h>


class GXMeshGeometry;
class GXMesh
{
	friend class GXMeshGeometry;

	private:
		GXUInt					referenceCount;
		GXMesh*					previous;
		GLsizeiptr				vboSize;

		static GXMesh*			top;
		GXMesh*					next;

		GXWChar*				meshFile;
		GLenum					vboUsage;

	public:
		GLsizei					totalVertices;
		GLuint					meshVBO;

	public:
		GXVoid AddReference ();
		GXVoid Release ();

		// Method return nullptr if mesh is procedure mesh.
		// Method returns valid zero terminated string if mesh is not procedure mesh.
		const GXWChar* GetMeshFileName () const;

		// Method automatically converts static mesh to procedure mesh if needed.
		GXVoid FillVBO ( const GXVoid* data, GLsizeiptr size, GLenum usage );

		// Method returns valid pointer if mesh was found.
		// Method returns nullptr if mesh was not found.
		static GXMesh* GXCALL Find ( const GXWChar* fileName );

		static GXUInt GXCALL GetTotalLoadedMeshes ( const GXWChar** lastMesh );

	private:
		// Creates procedure mesh.
		GXMesh ();

		// Creates static mesh.
		explicit GXMesh ( const GXWChar* fileName );

		~GXMesh ();

		GXBool LoadFromOBJ ( const GXWChar* fileName );
		GXBool LoadFromSTM ( const GXWChar* fileName );
		GXBool LoadFromSKM ( const GXWChar* fileName );
		GXBool LoadFromMESH ( const GXWChar* fileName );

		GXMesh ( const GXMesh &other ) = delete;
		GXMesh& operator = ( const GXMesh &other ) = delete;
};

class GXSkin
{
	friend class GXMeshGeometry;

	private:
		GXUInt			referenceCount;
		GXSkin*			previous;

		static GXSkin*	top;
		GXSkin*			next;

		GXWChar*		skinFile;

	public:
		GLsizei			totalVertices;
		GLuint			skinVBO;

	public:
		GXVoid AddReference ();
		GXVoid Release ();

		// Method returns valid pointer or nullptr.
		static GXSkin* GXCALL Find ( const GXWChar* fileName );
		static GXUInt GXCALL GetTotalLoadedSkins ( const GXWChar** lastSkin );

	private:
		explicit GXSkin ( const GXWChar* fileName );
		~GXSkin ();

		GXBool LoadFromSKM ( const GXWChar* fileName );
		GXBool LoadFromSKIN ( const GXWChar* fileName );

		GXSkin ( const GXSkin &other ) = delete;
		GXSkin& operator = ( const GXSkin &other ) = delete;
};

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

	private:
		GXVoid UpdateGraphicResources ();

		GXMeshGeometry ( const GXMeshGeometry &other ) = delete;
		GXMeshGeometry& operator = ( const GXMeshGeometry &other ) = delete;
};


#endif // GX_MESH_GEOMETRY
