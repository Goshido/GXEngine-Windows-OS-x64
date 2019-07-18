#include <GXEngine_Editor_Mobile/EMMeshActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_ALBEDO_TEXTURE                  L"Textures/System/Default_Diffuse.tga"
#define DEFAULT_NORMAL_TEXTURE                  L"Textures/Editor Mobile/Default Normals.tex"
#define DEFAULT_EMISSION_TEXTURE                L"Textures/System/Default_Diffuse.tga"
#define DEFAULT_PARAMETER_TEXTURE               L"Textures/System/Default_Diffuse.tga"

#define DEFAULT_EMISSION_SCALE                  0.0f
#define DEFAULT_ROUGHNESS_SCALE                 0.25f
#define DEFAULT_INDEX_OF_REFRACTION__SCALE      0.094f
#define DEFAULT_SPECULAR_INTENSITY              0.2f
#define DEFAULT_METALLIC_SCALE                  1.0f

//-------------------------------------------------------------------------------

EMMeshActor::EMMeshActor ( const GXWChar* name, const GXTransform& transform ):
    EMActor ( name, eEMActorType::Mesh, transform )
{
    _albedo.LoadImage ( DEFAULT_ALBEDO_TEXTURE, GX_TRUE, GX_FALSE );
    _normal.LoadImage ( DEFAULT_NORMAL_TEXTURE, GX_TRUE, GX_FALSE );
    _emission.LoadImage ( DEFAULT_EMISSION_TEXTURE, GX_TRUE, GX_FALSE );
    _parameter.LoadImage ( DEFAULT_PARAMETER_TEXTURE, GX_TRUE, GX_FALSE );

    _material.SetAlbedoTexture ( _albedo );
    _material.SetNormalTexture ( _normal );
    _material.SetEmissionTexture ( _emission );
    _material.SetParameterTexture ( _parameter );

    _material.SetEmissionColorScale ( DEFAULT_EMISSION_SCALE );
    _material.SetRoughnessScale ( DEFAULT_ROUGHNESS_SCALE );
    _material.SetIndexOfRefractionScale ( DEFAULT_INDEX_OF_REFRACTION__SCALE );
    _material.SetSpecularIntensityScale ( DEFAULT_SPECULAR_INTENSITY );
    _material.SetMetallicScale ( DEFAULT_METALLIC_SCALE );
}

EMMeshActor::~EMMeshActor ()
{
    // NOTHING
}

GXVoid EMMeshActor::OnDrawCommonPass ( GXFloat deltaTime )
{
    if ( !_isVisible ) return;

    EMRenderer& renderer = EMRenderer::GetInstance ();
    renderer.SetObjectMask ( this );

    GXRenderer& coreRenderer = GXRenderer::GetInstance ();

    _material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
    _material.SetExposure ( renderer.GetMotionBlurExposure () );
    _material.SetDeltaTime ( deltaTime );
    _material.SetScreenResolution ( static_cast<GXUShort> ( coreRenderer.GetWidth () ), static_cast<GXUShort> ( coreRenderer.GetHeight () ) );
    _material.Bind ( _transform );
    _mesh.Render ();
    _material.Unbind ();

    renderer.SetObjectMask ( nullptr );
    _transform.UpdateLastFrameModelMatrix ();
}

GXVoid EMMeshActor::SetMesh ( const GXWChar* meshFile )
{
    _mesh.LoadMesh ( meshFile );
}

EMCookTorranceCommonPassMaterial& EMMeshActor::GetMaterial ()
{
    return _material;
}
