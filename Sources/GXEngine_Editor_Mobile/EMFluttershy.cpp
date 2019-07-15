#include <GXEngine_Editor_Mobile/EMFluttershy.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>


#define SOLVER_ID                       0u

#define ROUGHNESS_SCALE                 0.8f
#define INDEX_OF_REFRACTION_SCALE       0.306f
#define SPECULAR_INTENSITY_SCALE        0.1f
#define METALLIC_SCALE                  0.0f

//---------------------------------------------------------------------------------------------------------------------

EMFluttershy::EMFluttershy ():
// mesh ( L"3D Models/Editor Mobile/Fluttershy.skm" ), animationSolverPlayer ( SOLVER_ID )
    _mesh ( L"Meshes/Editor Mobile/pilot.skm", L"Meshes/Editor Mobile/pilot.skm" ),
    _animationSolverPlayer ( static_cast<GXUShort> ( SOLVER_ID ) )
{
    // GXLoadNativeAnimation ( animationInfo, L"Animations/Editor Mobile/pony animation.ani" );
    // GXLoadNativeAnimation ( animationInfo, L"Animations/Editor Mobile/default.ani" );
    GXLoadNativeAnimation ( _animationInfo, L"Animations/Editor Mobile/pilot.ani" );
    // skeleton.LoadFromSkm ( L"Meshes/Editor Mobile/Fluttershy.skm" );
    _skeleton.LoadFromSkm ( L"Meshes/Editor Mobile/pilot.skm" );
    _animationSolverPlayer.SetAnimationSequence ( &_animationInfo );
    _animationSolverPlayer.SetAnimationMultiplier ( 1.0f );
    // animationSolverPlayer.EnableNormalization ();

    // albedoTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Fluttershy_Diffuse.tga", GX_TRUE, GL_REPEAT, GX_FALSE );
    _albedoTexture.LoadImage ( L"Textures/Editor Mobile/soldier.png", GX_TRUE, GX_TRUE );
    _normalTexture.LoadImage ( L"Textures/Editor Mobile/Default Normals.tex", GX_TRUE, GX_FALSE );
    _emissionTexture.LoadImage ( L"Textures/Editor Mobile/Default Emission.tex", GX_TRUE, GX_FALSE );
    _parameterTexture.LoadImage ( L"Textures/System/Default_Diffuse.tga", GX_TRUE, GX_FALSE );

    _material.SetAlbedoTexture ( _albedoTexture );
    _material.SetNormalTexture ( _normalTexture );
    _material.SetEmissionTexture ( _emissionTexture );
    _material.SetParameterTexture ( _parameterTexture );

    _material.SetRoughnessScale ( ROUGHNESS_SCALE );
    _material.SetIndexOfRefractionScale ( INDEX_OF_REFRACTION_SCALE );
    _material.SetSpecularIntensityScale ( SPECULAR_INTENSITY_SCALE );
    _material.SetMetallicScale ( METALLIC_SCALE );
}

EMFluttershy::~EMFluttershy ()
{
    _animationInfo.Cleanup ();
}

GXVoid EMFluttershy::Render ( GXFloat deltaTime )
{
    EMRenderer& renderer = EMRenderer::GetInstance ();
    renderer.SetObjectMask ( nullptr );
    GXRenderer& coreRenderer = GXRenderer::GetInstance ();

    _material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
    _material.SetExposure ( renderer.GetMotionBlurExposure () );
    _material.SetScreenResolution ( static_cast<GXUShort> ( coreRenderer.GetWidth () ), static_cast<GXUShort> ( coreRenderer.GetHeight () ) );
    _material.SetDeltaTime ( deltaTime );
    _material.Bind ( _mesh );
    _mesh.Render ();
    _material.Unbind ();

    _mesh.UpdateLastFrameModelMatrix ();
}

GXVoid EMFluttershy::UpdatePose ( GXFloat deltaTime )
{
    _animationSolverPlayer.Update ( deltaTime );
    _skeleton.UpdatePose ( _animationSolverPlayer, deltaTime );
    _mesh.UpdatePose ( _skeleton );
}

const GXSkeleton& EMFluttershy::GetSkeleton () const
{
    return _skeleton;
}

GXVoid EMFluttershy::TransformUpdated ()
{
    _mesh.SetScale ( _currentFrameScale );
    _mesh.SetRotation ( _currentFrameRotationMatrix );
    _mesh.SetLocation ( _currentFrameLocation );
}
