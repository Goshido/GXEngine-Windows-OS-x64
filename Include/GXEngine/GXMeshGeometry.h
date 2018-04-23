// version 1.1

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include "GXSkeleton.h"
#include "GXSkinningMaterial.h"
#include <GXCommon/GXMath.h>


class GXMesh
{
	private:
		GXMesh*					next;
		GXMesh*					previous;
		static GXMesh*			top;

		GXUInt					referenceCount;

		GXWChar*				meshFile;
		GLsizeiptr				vboSize;
		GLenum					vboUsage;

	public:
		GLsizei					totalVertices;
		GLuint					meshVBO;

	public:
		// Creates procedure mesh.
		GXMesh ();

		// Creates static mesh.
		explicit GXMesh ( const GXWChar* fileName );

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
		~GXMesh ();

		GXBool LoadFromOBJ ( const GXWChar* fileName );
		GXBool LoadFromSTM ( const GXWChar* fileName );
		GXBool LoadFromSKM ( const GXWChar* fileName );
		GXBool LoadFromMESH ( const GXWChar* fileName );
};

class GXSkin
{
	private:
		GXSkin*			next;
		GXSkin*			previous;
		static GXSkin*	top;

		GXUInt			referenceCount;
		GXWChar*		skinFile;

	public:
		GLsizei			totalVertices;
		GLuint			skinVBO;

		explicit GXSkin ( const GXWChar* fileName );

		GXVoid AddReference ();
		GXVoid Release ();

		// Method returns valid pointer or nullptr.
		static GXSkin* GXCALL Find ( const GXWChar* fileName );
		static GXUInt GXCALL GetTotalLoadedSkins ( const GXWChar** lastSkin );

	private:
		~GXSkin ();

		GXBool LoadFromSKM ( const GXWChar* fileName );
		GXBool LoadFromSKIN ( const GXWChar* fileName );
};

enum class eGXMeshStreamIndex : GLuint
{
	CurrenVertex		= 0,
	UV					= 1,
	Normal				= 2,
	Tangent				= 3,
	Bitangent			= 4,
	SkinningIndices		= 5,
	SkinnngWeights		= 6,
	LastFrameVertex		= 7,
	Color				= 8
};

class GXMeshGeometry
{
	private:
		GXMesh*					mesh;
		GLuint					meshVAO;
		GLenum					topology;

		GXSkin*					skin;
		GLuint					poseVAO[ 2 ];
		GXMesh*					pose[ 2 ];
		GXUByte					skinningSwitchIndex;
		GXSkinningMaterial*		skinningMaterial;

		GXAABB					boundsLocal;

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
};


#endif // GX_MESH_GEOMETRY
