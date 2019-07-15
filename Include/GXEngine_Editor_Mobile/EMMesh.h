#ifndef EM_MESH
#define EM_MESH


#include <GXEngine/GXTransform.h>
#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXStrings.h>


class EMMesh final : public GXTransform, public GXRenderable
{
    private:
        const GXString      _meshFileName;
        const GXString      _skinFileName;
        GXMeshGeometry      _meshGeometry;
        GXAABB              _boundsWorld;

    public:
        explicit EMMesh ( const GXWChar* meshFileName );
        explicit EMMesh ( const GXWChar* meshFileName, const GXWChar* skinFileName );
        ~EMMesh () override;

        GXVoid Render () override;

        GXVoid UpdatePose ( GXSkeleton &skeleton );

    protected:
        GXVoid InitGraphicResources () override;
        GXVoid TransformUpdated () override;

    private:
        EMMesh () = delete;
        EMMesh ( const EMMesh &other ) = delete;
        EMMesh& operator = ( const EMMesh &other ) = delete;
};


#endif // EM_MESH
