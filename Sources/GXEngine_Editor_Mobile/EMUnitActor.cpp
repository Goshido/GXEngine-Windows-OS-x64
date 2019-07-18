#include <GXEngine_Editor_Mobile/EMUnitActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


#define ALBEDO_TEXTURE              L"Textures/System/Checker.tga"
#define NORMAL_TEXTURE              L"Textures/Editor Mobile/Default Normals.tex"
#define EMISSION_TEXTURE            L"Textures/System/Checker.tga"
#define PARAMETER_TEXTURE           L"Textures/Editor Mobile/Default Diffuse.tex"

#define ALBEDO_TEXTURE_SCALE        0.25f
#define EMISSION_TEXTURE_SCALE      0.25f
#define EMISSION_SCALE              0.0777f
#define SPECULAR_INTENSITY_SCALE    0.2f
#define METALLIC_SCALE              1.0f

#define CUBE_MESH                   L"Meshes/System/Unit Cube.stm"

//---------------------------------------------------------------------------------------------------------------------

EMUnitActor::EMUnitActor ( const GXWChar* name, const GXTransform &transform ):
    EMActor ( name, eEMActorType::UnitCube, transform ),
    _mesh ( CUBE_MESH ),
    _albedoTexture ( ALBEDO_TEXTURE, GX_FALSE, GX_TRUE ),
    _normalTexture ( NORMAL_TEXTURE, GX_FALSE, GX_FALSE ),
    _emissionTexture ( EMISSION_TEXTURE, GX_FALSE, GX_FALSE ),
    _parameterTexture ( PARAMETER_TEXTURE, GX_FALSE, GX_FALSE )
{
    OnTransformChanged ();

    _commonPassMaterial.SetAlbedoTexture ( _albedoTexture );
    _commonPassMaterial.SetNormalTexture ( _normalTexture );
    _commonPassMaterial.SetEmissionTexture ( _emissionTexture );
    _commonPassMaterial.SetParameterTexture ( _parameterTexture );

    _commonPassMaterial.SetAlbedoTextureScale ( ALBEDO_TEXTURE_SCALE, ALBEDO_TEXTURE_SCALE );
    _commonPassMaterial.SetEmissionTextureScale ( EMISSION_TEXTURE_SCALE, EMISSION_TEXTURE_SCALE );
    _commonPassMaterial.SetEmissionColorScale ( EMISSION_SCALE );
    _commonPassMaterial.SetSpecularIntensityScale ( SPECULAR_INTENSITY_SCALE );
    _commonPassMaterial.SetMetallicScale ( METALLIC_SCALE );
}

EMUnitActor::~EMUnitActor ()
{
    // NOTHING
}

GXVoid EMUnitActor::OnDrawCommonPass ( GXFloat deltaTime )
{
    if ( !_isVisible ) return;

    EMRenderer& renderer = EMRenderer::GetInstance ();
    renderer.SetObjectMask ( this );

    const GXRenderer& coreRenderer = GXRenderer::GetInstance ();

    _commonPassMaterial.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
    _commonPassMaterial.SetExposure ( renderer.GetMotionBlurExposure () );
    _commonPassMaterial.SetDeltaTime ( deltaTime );
    _commonPassMaterial.SetScreenResolution ( static_cast<GXUShort> ( coreRenderer.GetWidth () ), static_cast<GXUShort> ( coreRenderer.GetHeight () ) );
    _commonPassMaterial.Bind ( _mesh );
    _mesh.Render ();
    _commonPassMaterial.Unbind ();

    _mesh.UpdateLastFrameModelMatrix ();
}

GXVoid EMUnitActor::OnSave ( GXUByte** /*data*/ )
{
    // TODO
}

GXVoid EMUnitActor::OnLoad ( const GXUByte* /*data*/ )
{
    // TODO
}

GXUPointer EMUnitActor::OnRequeredSaveSize () const
{
    // TODO

    return 0u;
}

GXVoid EMUnitActor::OnTransformChanged ()
{
    _mesh.SetLocation ( _transform.GetLocation () );
    _mesh.SetRotation ( _transform.GetRotation () );
}
