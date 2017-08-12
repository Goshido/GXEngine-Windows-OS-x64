#include <GXEngine_Editor_Mobile/EMMeshActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_ALBEDO_TEXTURE					L"Textures/System/Default_Diffuse.tga"
#define DEFAULT_NORMAL_TEXTURE					L"Textures/Editor Mobile/Default Normals.tex"
#define DEFAULT_EMISSION_TEXTURE				L"Textures/System/Default_Diffuse.tga"
#define DEFAULT_PARAMETER_TEXTURE				L"Textures/System/Default_Diffuse.tga"

#define DEFAULT_EMISSION_SCALE					0.0f
#define DEFAULT_ROUGHNESS_SCALE					0.25f
#define DEFAULT_INDEX_OF_REFRACTION__SCALE		0.094f
#define DEFAULT_SPECULAR_INTENSITY				0.2f
#define DEFAULT_METALLIC_SCALE					1.0f


EMMeshActor::EMMeshActor ( const GXWChar* name, const GXTransform& transform ):
EMActor ( name, eEMActorType::Mesh, transform )
{
	albedo = GXTexture2D::LoadTexture ( DEFAULT_ALBEDO_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE );
	normal = GXTexture2D::LoadTexture ( DEFAULT_NORMAL_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE );
	emission = GXTexture2D::LoadTexture ( DEFAULT_EMISSION_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE );
	parameter = GXTexture2D::LoadTexture ( DEFAULT_PARAMETER_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE );

	material.SetAlbedoTexture ( albedo );
	material.SetNormalTexture ( normal );
	material.SetEmissionTexture ( emission );
	material.SetParameterTexture ( parameter );

	material.SetEmissionColorScale ( DEFAULT_EMISSION_SCALE );
	material.SetRoughnessScale ( DEFAULT_ROUGHNESS_SCALE );
	material.SetIndexOfRefractionScale ( DEFAULT_INDEX_OF_REFRACTION__SCALE );
	material.SetSpecularIntensityScale ( DEFAULT_SPECULAR_INTENSITY );
	material.SetMetallicScale ( DEFAULT_METALLIC_SCALE );
}

EMMeshActor::~EMMeshActor ()
{
	GXTexture2D texture;

	GXTexture2D::RemoveTexture ( albedo );
	GXTexture2D::RemoveTexture ( normal );
	GXTexture2D::RemoveTexture ( emission );
	GXTexture2D::RemoveTexture ( parameter );

	GXMeshGeometry::RemoveMeshGeometry ( mesh );
}

GXVoid EMMeshActor::OnDrawCommonPass ( GXFloat deltaTime )
{
	if ( !isVisible ) return;

	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.SetObjectMask ( this );

	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
	material.SetExposure ( renderer.GetMotionBlurExposure () );
	material.SetDeltaTime ( deltaTime );
	material.SetScreenResolution ( (GXUShort)coreRenderer.GetWidth (), (GXUShort)coreRenderer.GetHeight () );
	material.Bind ( transform );
	mesh.Render ();
	material.Unbind ();

	renderer.SetObjectMask ( nullptr );
	transform.UpdateLastFrameModelMatrix ();
}

GXVoid EMMeshActor::SetMesh ( const GXWChar* meshFile )
{
	GXMeshGeometry::RemoveMeshGeometry ( mesh );

	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, meshFile );

	if ( GXWcscmp ( extension, L"obj" ) == 0 || GXWcscmp ( extension, L"OBJ" ) == 0 )
		mesh = GXMeshGeometry::LoadFromObj ( meshFile );
	else if ( GXWcscmp ( extension, L"stm" ) == 0 || GXWcscmp ( extension, L"STM" ) == 0 )
		mesh = GXMeshGeometry::LoadFromStm ( meshFile );
	else if ( GXWcscmp ( extension, L"skm" ) == 0 || GXWcscmp ( extension, L"SKM" ) == 0 )
		mesh = GXMeshGeometry::LoadFromStm ( meshFile );
	else
	{
		const GXWChar* message = GXLocale::GetInstance ().GetString ( L"EMMeshActor::SetMesh::Error - Can't load mesh %s. Unknown format %s" );
		GXLogW ( message, meshFile, extension );
	}

	free ( extension );
}

EMCookTorranceCommonPassMaterial& EMMeshActor::GetMaterial ()
{
	return material;
}
