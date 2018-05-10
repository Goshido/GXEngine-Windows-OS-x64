#include <GXEngine_Editor_Mobile/EMPhysicsDrivenActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXBoxShape.h>
#include <GXCommon/GXStrings.h>
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

#define SHAPE_COLOR_RED							115u
#define SHAPE_COLOR_GREEN						255u
#define SHAPE_COLOR_BLUE						0u
#define SHAPE_COLOR_ALPHA						77u

//---------------------------------------------------------------------------------------------------------------------

EMPhysicsDrivenActor::EMPhysicsDrivenActor ( const GXWChar* name, const GXTransform& transform ):
	EMActor ( name, eEMActorType::PhysicsDrivenActor, transform ),
	wireframeMaterial ( nullptr ),
	albedo ( DEFAULT_ALBEDO_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE ),
	normal ( DEFAULT_NORMAL_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE ),
	emission ( DEFAULT_EMISSION_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE ),
	parameter ( DEFAULT_PARAMETER_TEXTURE, GX_TRUE, GL_REPEAT, GX_FALSE )
{
	rigidBody.SetLocation ( transform.GetLocation () );
	rigidBody.SetRotaton ( GXQuat ( transform.GetRotation () ) );

	material.SetAlbedoTexture ( albedo );
	material.SetNormalTexture ( normal );
	material.SetEmissionTexture ( emission );
	material.SetParameterTexture ( parameter );

	material.SetEmissionColorScale ( DEFAULT_EMISSION_SCALE );
	material.SetRoughnessScale ( DEFAULT_ROUGHNESS_SCALE );
	material.SetIndexOfRefractionScale ( DEFAULT_INDEX_OF_REFRACTION__SCALE );
	material.SetSpecularIntensityScale ( DEFAULT_SPECULAR_INTENSITY );
	material.SetMetallicScale ( DEFAULT_METALLIC_SCALE );

	GXPhysicsEngine::GetInstance ().GetWorld ().RegisterRigidBody ( rigidBody );
	rigidBody.SetOnTransformChangedCallback ( this, &EMPhysicsDrivenActor::OnRigidBodyTransformChanged );
}

EMPhysicsDrivenActor::~EMPhysicsDrivenActor ()
{
	GXPhysicsEngine::GetInstance ().GetWorld ().UnregisterRigidBody ( rigidBody );

	if ( !wireframeMaterial ) return;

	delete wireframeMaterial;
}

GXVoid EMPhysicsDrivenActor::OnDrawCommonPass ( GXFloat deltaTime )
{
	if ( !isVisible ) return;

	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.SetObjectMask ( this );

	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
	material.SetExposure ( renderer.GetMotionBlurExposure () );
	material.SetDeltaTime ( deltaTime );
	material.SetScreenResolution ( static_cast<GXUShort> ( coreRenderer.GetWidth () ), static_cast<GXUShort> ( coreRenderer.GetHeight () ) );
	material.Bind ( transform );
	mesh.Render ();
	material.Unbind ();

	renderer.SetObjectMask ( nullptr );
	transform.UpdateLastFrameModelMatrix ();
}

GXVoid EMPhysicsDrivenActor::OnDrawHudColorPass ()
{
	if ( !wireframeMaterial ) return;

	GXTransform tr;
	tr.SetLocation ( rigidBody.GetLocation () );
	tr.SetRotation ( rigidBody.GetRotation () );

	GXShape& shape = rigidBody.GetShape ();

	switch ( shape.GetType () )
	{
		case eGXShapeType::Box:
		{
			GXBoxShape& boxShape = static_cast<GXBoxShape&> ( shape );
			tr.SetScale ( boxShape.GetWidth (), boxShape.GetHeight (), boxShape.GetDepth () );

			wireframeMaterial->Bind ( tr );
			unitCubeMesh.Render ();
			wireframeMaterial->Unbind ();
		}
		break;

		case eGXShapeType::Rectangle:
			// NOT SUPPORTED YET
		return;

		case eGXShapeType::Sphere:
		{
			GXSphereShape& sphereShape = static_cast<GXSphereShape&> ( shape );
			GXFloat s = 2.0f * sphereShape.GetRadius ();
			tr.SetScale ( s, s, s );

			wireframeMaterial->Bind ( tr );
			unitSphereMesh.Render ();
			wireframeMaterial->Unbind ();
		}
		break;

		default:
			// NOTHING
		break;
	}
}

GXVoid EMPhysicsDrivenActor::OnTransformChanged ()
{
	// NOTHING
}

GXVoid EMPhysicsDrivenActor::SetMesh ( const GXWChar* meshFile )
{
	mesh.LoadMesh ( meshFile );
}

GXRigidBody& EMPhysicsDrivenActor::GetRigidBody ()
{
	return rigidBody;
}

EMCookTorranceCommonPassMaterial& EMPhysicsDrivenActor::GetMaterial ()
{
	return material;
}

GXVoid EMPhysicsDrivenActor::EnablePhysicsDebug ()
{
	if ( wireframeMaterial ) return;

	wireframeMaterial = new EMWireframeMaterial ();
	wireframeMaterial->SetColor ( SHAPE_COLOR_RED, SHAPE_COLOR_GREEN, SHAPE_COLOR_BLUE, SHAPE_COLOR_ALPHA );

	unitSphereMesh.LoadMesh ( L"Meshes/System/Unit Sphere.obj" );
	unitCubeMesh.LoadMesh ( L"Meshes/System/Unit Cube.stm" );
}

GXVoid EMPhysicsDrivenActor::DisablePhysicsDebug ()
{
	if ( !wireframeMaterial ) return;

	GXSafeDelete ( wireframeMaterial );
}

GXVoid GXCALL EMPhysicsDrivenActor::OnRigidBodyTransformChanged ( GXVoid* handler, const GXRigidBody& rigidBody )
{
	EMPhysicsDrivenActor* physicsDrivenActor = static_cast<EMPhysicsDrivenActor*> ( handler );
	physicsDrivenActor->transform.SetLocation ( rigidBody.GetLocation () );
	physicsDrivenActor->transform.SetRotation ( rigidBody.GetRotation () );

	physicsDrivenActor->OnTransformChanged ();
}
