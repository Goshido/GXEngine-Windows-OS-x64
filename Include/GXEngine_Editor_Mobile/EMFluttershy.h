#ifndef EM_FLUTTERSHY
#define EM_FLUTTERSHY


#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXAnimationSolverPlayer.h>


class EMFluttershy final : public GXTransform
{
    private:
        EMMesh                              _mesh;
        GXSkeleton                          _skeleton;
        GXAnimationSolverPlayer             _animationSolverPlayer;
        GXAnimationInfo                     _animationInfo;

        GXTexture2D                         _albedoTexture;
        GXTexture2D                         _normalTexture;
        GXTexture2D                         _emissionTexture;
        GXTexture2D                         _parameterTexture;

        EMCookTorranceCommonPassMaterial    _material;

    public:
        EMFluttershy ();
        ~EMFluttershy () override;

        GXVoid Render ( GXFloat deltaTime );
        GXVoid UpdatePose ( GXFloat deltaTime );

        const GXSkeleton& GetSkeleton () const;

    protected:
        GXVoid TransformUpdated () override;

    private:
        EMFluttershy ( const EMFluttershy &other ) = delete;
        EMFluttershy& operator = ( const EMFluttershy &other ) = delete;
};


#endif // EM_FLUTTERSHY
