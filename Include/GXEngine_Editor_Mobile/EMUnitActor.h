#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"
#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXTexture2D.h>


class EMUnitActor final : public EMActor
{
    private:
        EMMesh                              _mesh;

        GXTexture2D                         _albedoTexture;
        GXTexture2D                         _normalTexture;
        GXTexture2D                         _emissionTexture;
        GXTexture2D                         _parameterTexture;

        EMCookTorranceCommonPassMaterial    _commonPassMaterial;

    public:
        explicit EMUnitActor ( const GXWChar* name, const GXTransform &transform );
        ~EMUnitActor () override;

        GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;

        GXVoid OnSave ( GXUByte** data ) override;
        GXVoid OnLoad ( const GXUByte* data ) override;
        GXUPointer OnRequeredSaveSize () const override;
        GXVoid OnTransformChanged () override;

    private:
        EMUnitActor () = delete;
        EMUnitActor ( const EMUnitActor &other ) = delete;
        EMUnitActor& operator = ( const EMUnitActor &other ) = delete;
};


#endif // EM_UNIT_ACTOR
