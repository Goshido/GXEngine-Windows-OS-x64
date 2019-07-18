#ifndef EM_ENVIRONMENT_MAP_MATERIAL
#define EM_ENVIRONMENT_MAP_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMEnvironmentMapMaterial final : public GXMaterial
{
    private:
        GXTextureCubeMap*       _environmentTexture;
        GXTexture2D*            _depthTexture;

        GXSampler               _environmentSampler;
        GXSampler               _depthSampler;

        GXVec2                  _screenResolution;
        GXVec2                  _inverseScreenResolution;

        GXFloat                 _inverseDeltaTime;
        GXFloat                 _environmentQuasiDistance;

        GLint                   _modelViewProjectionMatrixLocation;
        GLint                   _inverseScreenResolutionLocation;
        GLint                   _velocityBlurLocation;

    public:
        EMEnvironmentMapMaterial ();
        ~EMEnvironmentMapMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetEnvironmentMap ( GXTextureCubeMap &cubeMap );
        GXVoid SetDepthTexture ( GXTexture2D &texture );

        GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
        GXVoid SetDeltaTime ( GXFloat seconds );

        // Distance to environment map border to calculate positions and then velocity blur.
        GXVoid SetEnvironmentQuasiDistance ( GXFloat meters );

    private:
        EMEnvironmentMapMaterial ( const EMEnvironmentMapMaterial &other ) = delete;
        EMEnvironmentMapMaterial& operator = ( const EMEnvironmentMapMaterial &other ) = delete;
};


#endif // EM_ENVIRONMENT_MAP_MATERIAL
