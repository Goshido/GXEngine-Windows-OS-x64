#ifndef EM_SCALE_GISMO
#define EM_SCALE_GISMO


#include "EMMesh.h"
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUnlitTexture2DMaterial.h>


class EMScaleGismo final : public GXTransform
{
    private:
        GXBool                      _isVisible;
        EMMesh                      _mesh;
        GXTexture2D                 _texture;

        GXUnlitTexture2DMaterial    _unlitMaterial;

    public:
        EMScaleGismo ();
        ~EMScaleGismo () override;

        GXVoid Hide ();
        GXVoid Show ();

        GXVoid Render ();

    protected:
        GXVoid TransformUpdated () override;

    private:
        EMScaleGismo ( const EMScaleGismo &other ) = delete;
        EMScaleGismo& operator = ( const EMScaleGismo &other ) = delete;
};


#endif // EM_SCALE_GISMO
