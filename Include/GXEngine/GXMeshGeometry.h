//version 1.1

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

	public:
		GLsizei					totalVertices;
		GLuint					meshVBO;

	public:
		GXMesh ();
		explicit GXMesh ( const GXWChar* fileName );

		GXVoid AddReference ();
		GXVoid Release ();

		const GXWChar* GetMeshFileName () const;
		GXVoid FillVBO ( const GXVoid* data, GLsizeiptr size, GLenum usage );

		// Method returns valid pointer or nullptr.
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

		const GXWChar* GetSkinFileName () const;

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
		GLuint					skinningVAO;
		GLuint					poseVAO[ 2 ];
		GLuint					poseVBO[ 2 ];
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

		//Unloads previous mesh if it exists.
		GXBool LoadMesh ( const GXWChar* fileName );

		//Unloads previous skin if it exists.
		GXBool LoadSkin ( const GXWChar* fileName );

	private:
		GXVoid UpdateGraphicResources ();
};


#endif //GX_MESH_GEOMETRY
