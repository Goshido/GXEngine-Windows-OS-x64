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


EMUnitActor::EMUnitActor ( const GXWChar* name, const GXTransform &transform ):
EMActor ( name, eEMActorType::UnitCube, transform ),
mesh ( L"Meshes/System/Unit Cube.stm" )
{
	OnTransformChanged ();

	albedoTexture = GXTexture2D::LoadTexture ( ALBEDO_TEXTURE, GX_FALSE, GL_REPEAT, GX_TRUE );
	normalTexture = GXTexture2D::LoadTexture ( NORMAL_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	emissionTexture = GXTexture2D::LoadTexture ( EMISSION_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	parameterTexture = GXTexture2D::LoadTexture ( PARAMETER_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );

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
	GXTexture2D::RemoveTexture ( albedoTexture );
	GXTexture2D::RemoveTexture ( normalTexture );
	GXTexture2D::RemoveTexture ( emissionTexture );
	GXTexture2D::RemoveTexture ( parameterTexture );
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
	commonPassMaterial.SetScreenResolution ( (GXUShort)coreRenderer.GetWidth (), (GXUShort)coreRenderer.GetHeight () );
	commonPassMaterial.Bind ( mesh );
	mesh.Render ();
	commonPassMaterial.Unbind ();

	mesh.UpdateLastFrameModelMatrix ();
}

GXVoid EMUnitActor::OnSave ( GXUByte** data )
{
	EMActorHeader* header = (EMActorHeader*)data;

	header->type = (GXUBigInt)type;
	memcpy ( &header->origin, &transform, sizeof ( GXMat4 ) );
	header->isVisible = isVisible;
	header->nameOffset = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	GXUPointer nameSize = GXToUTF8 ( &nameUTF8, name );

	memcpy ( data + sizeof ( EMActorHeader ), nameUTF8, nameSize );
	free ( nameUTF8 );

	header->size = sizeof ( EMActorHeader ) + nameSize;
}

GXVoid EMUnitActor::OnLoad ( const GXUByte* data )
{
	EMActorHeader* header = (EMActorHeader*)data;

	isVisible = header->isVisible;

	GXSafeFree ( name );
	GXToWcs ( &name, (GXUTF8*)( data + header->nameOffset ) );

	memcpy ( &transform, &header->origin, sizeof ( GXMat4 ) );
}

GXUPointer EMUnitActor::OnRequeredSaveSize () const
{
	GXUPointer total = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	total += GXToUTF8 ( &nameUTF8, name );

	free ( nameUTF8 );

	return total;
}

GXVoid EMUnitActor::OnTransformChanged ()
{
	mesh.SetLocation ( transform.GetLocation () );
	mesh.SetRotation ( transform.GetRotation () );
}
