#include "EMActor.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXMeshGeometry.h>


class EMMeshActor final : public EMActor
{
    private:
        GXMeshGeometry                      _mesh;
        GXTexture2D                         _albedo;
        GXTexture2D                         _normal;
        GXTexture2D                         _emission;
        GXTexture2D                         _parameter;
        EMCookTorranceCommonPassMaterial    _material;

    public:
        explicit EMMeshActor ( const GXWChar* name, const GXTransform& transform );
        ~EMMeshActor () override;

        GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;

        GXVoid SetMesh ( const GXWChar* meshFile );
        EMCookTorranceCommonPassMaterial& GetMaterial ();

    private:
        EMMeshActor () = delete;
        EMMeshActor ( const EMMeshActor &other ) = delete;
        EMMeshActor& operator = ( const EMMeshActor &other ) = delete;
};
