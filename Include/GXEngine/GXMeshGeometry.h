// version 1.9

#ifndef GX_MESH_GEOMETRY
#define GX_MESH_GEOMETRY


#include "GXOpenGL.h"
#include "GXSkeleton.h"
#include "GXSkinningMaterial.h"
#include <GXCommon/GXStrings.h>


enum class eGXMeshStreamIndex : GLuint
{
    CurrenVertex        = 0u,
    UV                  = 1u,
    Normal              = 2u,
    Tangent             = 3u,
    Bitangent           = 4u,
    SkinningIndices     = 5u,
    SkinningWeights     = 6u,
    LastFrameVertex     = 7u,
    Color               = 8u
};

class GXMesh;
class GXSkin;
class GXMeshGeometry final : public GXMemoryInspector
{
    private:
        GXMesh*                 _mesh;
        GLuint                  _meshVAO;
        GLenum                  _topology;

        GXSkin*                 _skin;
        GXUByte                 _skinningSwitchIndex;
        GXSkinningMaterial*     _skinningMaterial;

        GXAABB                  _boundsLocal;

        GLuint                  _poseVAO[ 2u ];
        GXMesh*                 _pose[ 2u ];

    public:
        GXMeshGeometry ();
        ~GXMeshGeometry () override;

        GXVoid Render();

        GXVoid SetBoundsLocal ( const GXAABB& bounds );
        const GXAABB& GetBoundsLocal () const;

        GXVoid SetTotalVertices ( GLsizei totalVertices );
        GXVoid FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage );
        GXVoid SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset );

        GXVoid SetTopology ( GLenum newTopology );

        GXVoid UpdatePose ( const GXSkeleton &skeleton );

        // Unloads previous mesh if it exists.
        GXBool LoadMesh ( const GXString &fileName );

        // Unloads previous skin if it exists.
        GXBool LoadSkin ( const GXString &fileName );

        static GXUInt GXCALL GetTotalLoadedMeshes ( GXString &lastMesh );
        static GXUInt GXCALL GetTotalLoadedSkins ( GXString &lastSkin );

    private:
        GXVoid UpdateGraphicResources ();

        GXMeshGeometry ( const GXMeshGeometry &other ) = delete;
        GXMeshGeometry& operator = ( const GXMeshGeometry &other ) = delete;
};


#endif // GX_MESH_GEOMETRY
