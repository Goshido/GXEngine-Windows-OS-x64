#include <GXEngine_Editor_Mobile/EMPhysicsDrivenActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXBoxShape.h>
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

#define SHAPE_COLOR_RED                         115u
#define SHAPE_COLOR_GREEN                       255u
#define SHAPE_COLOR_BLUE                        0u
#define SHAPE_COLOR_ALPHA                       77u

//---------------------------------------------------------------------------------------------------------------------

EMPhysicsDrivenActor::EMPhysicsDrivenActor ( const GXWChar* name, const GXTransform& transform ):
    EMActor ( name, eEMActorType::PhysicsDrivenActor, transform ),
    _wireframeMaterial ( nullptr ),
    _albedo ( DEFAULT_ALBEDO_TEXTURE, GX_TRUE, GX_FALSE ),
    _normal ( DEFAULT_NORMAL_TEXTURE, GX_TRUE, GX_FALSE ),
    _emission ( DEFAULT_EMISSION_TEXTURE, GX_TRUE, GX_FALSE ),
    _parameter ( DEFAULT_PARAMETER_TEXTURE, GX_TRUE, GX_FALSE )
{
    _rigidBody.SetLocation ( transform.GetLocation () );
    _rigidBody.SetRotaton ( GXQuat ( transform.GetRotation () ) );

    _material.SetAlbedoTexture ( _albedo );
    _material.SetNormalTexture ( _normal );
    _material.SetEmissionTexture ( _emission );
    _material.SetParameterTexture ( _parameter );

    _material.SetEmissionColorScale ( DEFAULT_EMISSION_SCALE );
    _material.SetRoughnessScale ( DEFAULT_ROUGHNESS_SCALE );
    _material.SetIndexOfRefractionScale ( DEFAULT_INDEX_OF_REFRACTION__SCALE );
    _material.SetSpecularIntensityScale ( DEFAULT_SPECULAR_INTENSITY );
    _material.SetMetallicScale ( DEFAULT_METALLIC_SCALE );

    GXPhysicsEngine::GetInstance ().GetWorld ().RegisterRigidBody ( _rigidBody );
    _rigidBody.SetOnTransformChangedCallback ( this, &EMPhysicsDrivenActor::OnRigidBodyTransformChanged );
}

EMPhysicsDrivenActor::~EMPhysicsDrivenActor ()
{
    GXPhysicsEngine::GetInstance ().GetWorld ().UnregisterRigidBody ( _rigidBody );

    if ( !_wireframeMaterial ) return;

    delete _wireframeMaterial;
}

GXVoid EMPhysicsDrivenActor::OnDrawCommonPass ( GXFloat deltaTime )
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

GXVoid EMPhysicsDrivenActor::OnDrawHudColorPass ()
{
    if ( !_wireframeMaterial ) return;

    GXTransform tr;
    tr.SetLocation ( _rigidBody.GetLocation () );
    tr.SetRotation ( _rigidBody.GetRotation () );

    GXShape& shape = _rigidBody.GetShape ();

    switch ( shape.GetType () )
    {
        case eGXShapeType::Box:
        {
            GXBoxShape& boxShape = static_cast<GXBoxShape&> ( shape );
            tr.SetScale ( boxShape.GetWidth (), boxShape.GetHeight (), boxShape.GetDepth () );

            _wireframeMaterial->Bind ( tr );
            _unitCubeMesh.Render ();
            _wireframeMaterial->Unbind ();
        }
        break;

        case eGXShapeType::Rectangle:
            // NOT SUPPORTED YET
        return;

        case eGXShapeType::Sphere:
        {
            GXSphereShape& sphereShape = static_cast<GXSphereShape&> ( shape );
            const GXFloat s = 2.0f * sphereShape.GetRadius ();
            tr.SetScale ( s, s, s );

            _wireframeMaterial->Bind ( tr );
            _unitSphereMesh.Render ();
            _wireframeMaterial->Unbind ();
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
    _mesh.LoadMesh ( meshFile );
}

GXRigidBody& EMPhysicsDrivenActor::GetRigidBody ()
{
    return _rigidBody;
}

EMCookTorranceCommonPassMaterial& EMPhysicsDrivenActor::GetMaterial ()
{
    return _material;
}

GXVoid EMPhysicsDrivenActor::EnablePhysicsDebug ()
{
    if ( _wireframeMaterial ) return;

    _wireframeMaterial = new EMWireframeMaterial ();
    _wireframeMaterial->SetColor ( SHAPE_COLOR_RED, SHAPE_COLOR_GREEN, SHAPE_COLOR_BLUE, SHAPE_COLOR_ALPHA );

    _unitSphereMesh.LoadMesh ( L"Meshes/System/Unit Sphere.obj" );
    _unitCubeMesh.LoadMesh ( L"Meshes/System/Unit Cube.stm" );
}

GXVoid EMPhysicsDrivenActor::DisablePhysicsDebug ()
{
    if ( !_wireframeMaterial ) return;

    GXSafeDelete ( _wireframeMaterial );
}

GXVoid GXCALL EMPhysicsDrivenActor::OnRigidBodyTransformChanged ( GXVoid* handler, const GXRigidBody& rigidBody )
{
    EMPhysicsDrivenActor* physicsDrivenActor = static_cast<EMPhysicsDrivenActor*> ( handler );
    physicsDrivenActor->_transform.SetLocation ( rigidBody.GetLocation () );
    physicsDrivenActor->_transform.SetRotation ( rigidBody.GetRotation () );

    physicsDrivenActor->OnTransformChanged ();
}
