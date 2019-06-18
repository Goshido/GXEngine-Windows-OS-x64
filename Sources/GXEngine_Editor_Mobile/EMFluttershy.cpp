#include <GXEngine_Editor_Mobile/EMFluttershy.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>


#define SOLVER_ID					0u

#define ROUGHNESS_SCALE				0.8f
#define INDEX_OF_REFRACTION_SCALE	0.306f
#define SPECULAR_INTENSITY_SCALE	0.1f
#define METALLIC_SCALE				0.0f

//---------------------------------------------------------------------------------------------------------------------

EMFluttershy::EMFluttershy ():
// mesh ( L"3D Models/Editor Mobile/Fluttershy.skm" ), animationSolverPlayer ( SOLVER_ID )
	mesh ( L"Meshes/Editor Mobile/pilot.skm", L"Meshes/Editor Mobile/pilot.skm" ),
	animationSolverPlayer ( static_cast<GXUShort> ( SOLVER_ID ) )
{
	// GXLoadNativeAnimation ( animationInfo, L"Animations/Editor Mobile/pony animation.ani" );
	// GXLoadNativeAnimation ( animationInfo, L"Animations/Editor Mobile/default.ani" );
	GXLoadNativeAnimation ( animationInfo, L"Animations/Editor Mobile/pilot.ani" );
	// skeleton.LoadFromSkm ( L"Meshes/Editor Mobile/Fluttershy.skm" );
	skeleton.LoadFromSkm ( L"Meshes/Editor Mobile/pilot.skm" );
	animationSolverPlayer.SetAnimationSequence ( &animationInfo );
	animationSolverPlayer.SetAnimationMultiplier ( 1.0f );
	// animationSolverPlayer.EnableNormalization ();

	// albedoTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Fluttershy_Diffuse.tga", GX_TRUE, GL_REPEAT, GX_FALSE );
	albedoTexture.LoadImage ( L"Textures/Editor Mobile/soldier.png", GX_TRUE, GX_TRUE );
	normalTexture.LoadImage ( L"Textures/Editor Mobile/Default Normals.tex", GX_TRUE, GX_FALSE );
	emissionTexture.LoadImage ( L"Textures/Editor Mobile/Default Emission.tex", GX_TRUE, GX_FALSE );
	parameterTexture.LoadImage ( L"Textures/System/Default_Diffuse.tga", GX_TRUE, GX_FALSE );

	material.SetAlbedoTexture ( albedoTexture );
	material.SetNormalTexture ( normalTexture );
	material.SetEmissionTexture ( emissionTexture );
	material.SetParameterTexture ( parameterTexture );

	material.SetRoughnessScale ( ROUGHNESS_SCALE );
	material.SetIndexOfRefractionScale ( INDEX_OF_REFRACTION_SCALE );
	material.SetSpecularIntensityScale ( SPECULAR_INTENSITY_SCALE );
	material.SetMetallicScale ( METALLIC_SCALE );
}

EMFluttershy::~EMFluttershy ()
{
	animationInfo.Cleanup ();
}

GXVoid EMFluttershy::Render ( GXFloat deltaTime )
{
	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.SetObjectMask ( nullptr );
	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
	material.SetExposure ( renderer.GetMotionBlurExposure () );
	material.SetScreenResolution ( static_cast<GXUShort> ( coreRenderer.GetWidth () ), static_cast<GXUShort> ( coreRenderer.GetHeight () ) );
	material.SetDeltaTime ( deltaTime );
	material.Bind ( mesh );
	mesh.Render ();
	material.Unbind ();

	mesh.UpdateLastFrameModelMatrix ();
}

GXVoid EMFluttershy::UpdatePose ( GXFloat deltaTime )
{
	animationSolverPlayer.Update ( deltaTime );
	skeleton.UpdatePose ( animationSolverPlayer, deltaTime );
	mesh.UpdatePose ( skeleton );
}

const GXSkeleton& EMFluttershy::GetSkeleton () const
{
	return skeleton;
}

GXVoid EMFluttershy::TransformUpdated ()
{
	mesh.SetScale ( _currentFrameScale );
	mesh.SetRotation ( _currentFrameRotationMatrix );
	mesh.SetLocation ( _currentFrameLocation );
}
