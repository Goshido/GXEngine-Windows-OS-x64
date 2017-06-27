#include <GXEngine_Editor_Mobile/EMUnitActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXTexture.h>
#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


#define DIFFUSE_TEXTURE		L"Textures/System/Checker.tga"
#define NORMAL_TEXTURE		L"Textures/Editor Mobile/Default Normals.tex"
#define EMISSION_TEXTURE	L"Textures/Editor Mobile/Default Emission.tex"
#define PARAMETER_TEXTURE	L"Textures/Editor Mobile/Default Cook Torrance parameters.tga"


EMUnitActor::EMUnitActor ( const GXWChar* name, const GXMat4 &transform ):
EMActor ( name, EM_UNIT_ACTOR_CLASS, transform ),
mesh ( L"3D Models/Editor Mobile/Unit Cube.stm" )
{
	OnTransformChanged ();

	diffuseTexture = GXTexture::LoadTexture ( DIFFUSE_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	normalTexture = GXTexture::LoadTexture ( NORMAL_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	emissionTexture = GXTexture::LoadTexture ( EMISSION_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	parameterTexture = GXTexture::LoadTexture ( PARAMETER_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );

	EMRenderer& renderer = EMRenderer::GetInstance ();

	commonPassMaterial.SetAlbedoTexture ( diffuseTexture );
	commonPassMaterial.SetAlbedoTextureScale ( 0.25f, 0.25f );
	commonPassMaterial.SetNormalTexture ( normalTexture );
	commonPassMaterial.SetEmissionTexture ( emissionTexture );
	commonPassMaterial.SetParameterTexture ( parameterTexture );
}

EMUnitActor::~EMUnitActor ()
{
	GXTexture::RemoveTexture ( diffuseTexture );
	GXTexture::RemoveTexture ( normalTexture );
	GXTexture::RemoveTexture ( emissionTexture );
	GXTexture::RemoveTexture ( parameterTexture );
}

GXVoid EMUnitActor::OnDrawCommonPass ( GXFloat deltaTime )
{
	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.SetObjectMask ( (GXUPointer)this );

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

	header->type = type;
	memcpy ( &header->origin, &transform, sizeof ( GXMat4 ) );
	header->isVisible = isVisible;
	header->nameOffset = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	GXUInt nameSize = GXToUTF8 ( &nameUTF8, name );

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

GXUInt EMUnitActor::OnRequeredSaveSize ()
{
	GXUInt total = sizeof ( EMActorHeader );

	GXUTF8* nameUTF8;
	total += GXToUTF8 ( &nameUTF8, name );

	free ( nameUTF8 );

	return total;
}

GXVoid EMUnitActor::OnTransformChanged ()
{
	mesh.SetLocation ( transform.wv );

	GXMat4 cleanRotation;
	GXSetMat4ClearRotation ( cleanRotation, transform );

	mesh.SetRotation ( cleanRotation );
}
