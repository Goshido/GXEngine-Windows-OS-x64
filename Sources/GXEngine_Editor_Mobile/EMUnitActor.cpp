#include <GXEngine_Editor_Mobile/EMUnitActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


#define ALBEDO_TEXTURE				L"Textures/System/Checker.tga"
#define NORMAL_TEXTURE				L"Textures/Editor Mobile/Default Normals.tex"
#define EMISSION_TEXTURE			L"Textures/System/Checker.tga"
#define PARAMETER_TEXTURE			L"Textures/Editor Mobile/Default Diffuse.tex"

#define ALBEDO_TEXTURE_SCALE		0.25f
#define EMISSION_TEXTURE_SCALE		0.25f
#define EMISSION_SCALE				0.0777f
#define SPECULAR_INTENSITY_SCALE	0.2f
#define METALLIC_SCALE				1.0f

#define CUBE_MESH					L"Meshes/System/Unit Cube.stm"

//---------------------------------------------------------------------------------------------------------------------

EMUnitActor::EMUnitActor ( const GXWChar* name, const GXTransform &transform ):
	EMActor ( name, eEMActorType::UnitCube, transform ),
	mesh ( CUBE_MESH ),
	albedoTexture ( ALBEDO_TEXTURE, GX_FALSE, GL_REPEAT, GX_TRUE ),
	normalTexture ( NORMAL_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE ),
	emissionTexture ( EMISSION_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE ),
	parameterTexture ( PARAMETER_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE )
{
	OnTransformChanged ();
	
	commonPassMaterial.SetAlbedoTexture ( albedoTexture );
	commonPassMaterial.SetNormalTexture ( normalTexture );
	commonPassMaterial.SetEmissionTexture ( emissionTexture );
	commonPassMaterial.SetParameterTexture ( parameterTexture );

	commonPassMaterial.SetAlbedoTextureScale ( ALBEDO_TEXTURE_SCALE, ALBEDO_TEXTURE_SCALE );
	commonPassMaterial.SetEmissionTextureScale ( EMISSION_TEXTURE_SCALE, EMISSION_TEXTURE_SCALE );
	commonPassMaterial.SetEmissionColorScale ( EMISSION_SCALE );
	commonPassMaterial.SetSpecularIntensityScale ( SPECULAR_INTENSITY_SCALE );
	commonPassMaterial.SetMetallicScale ( METALLIC_SCALE );
}

EMUnitActor::~EMUnitActor ()
{
	// NOTHING
}

GXVoid EMUnitActor::OnDrawCommonPass ( GXFloat deltaTime )
{
	if ( !isVisible ) return;

	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.SetObjectMask ( this );

	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	commonPassMaterial.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
	commonPassMaterial.SetExposure ( renderer.GetMotionBlurExposure () );
	commonPassMaterial.SetDeltaTime ( deltaTime );
	commonPassMaterial.SetScreenResolution ( static_cast<GXUShort> ( coreRenderer.GetWidth () ), static_cast<GXUShort> ( coreRenderer.GetHeight () ) );
	commonPassMaterial.Bind ( mesh );
	mesh.Render ();
	commonPassMaterial.Unbind ();

	mesh.UpdateLastFrameModelMatrix ();
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
	mesh.SetLocation ( transform.GetLocation () );
	mesh.SetRotation ( transform.GetRotation () );
}
