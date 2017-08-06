#include <GXEngine_Editor_Mobile/EMFluttershy.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>


#define SOLVER_ID		0


EMFluttershy::EMFluttershy () :
mesh ( L"3D Models/Editor Mobile/Fluttershy.skm" ), animationSolverPlayer ( SOLVER_ID )
{
	GXLoadNativeAnimation ( L"Animations/Editor Mobile/Battle.ani", animationInfo );
	skeleton.LoadFromSkm ( L"3D Models/Editor Mobile/Fluttershy.skm" );
	animationSolverPlayer.SetAnimationSequence ( &animationInfo );
	animationSolverPlayer.SetAnimationMultiplier ( 0.8f );
	animationSolverPlayer.EnableNormalization ();

	albedoTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Fluttershy_Diffuse.tga", GX_TRUE, GL_REPEAT, GX_FALSE );
	//normalTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Brick_Wall_Normals.tga", GX_TRUE, GL_REPEAT );
	normalTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	emissionTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Default Emission.tex", GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	parameterTexture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Default Cook Torrance parameters.tga", GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );

	EMRenderer& renderer = EMRenderer::GetInstance ();

	material.SetAlbedoTexture ( albedoTexture );
	material.SetNormalTexture ( normalTexture );
	//material.SetNormalTextureScale ( 5.0f, 5.0f );
	material.SetEmissionTexture ( emissionTexture );
	material.SetParameterTexture ( parameterTexture );
}

EMFluttershy::~EMFluttershy ()
{
	animationInfo.Cleanup ();

	GXTexture2D::RemoveTexture ( albedoTexture );
	GXTexture2D::RemoveTexture ( normalTexture );
	GXTexture2D::RemoveTexture ( emissionTexture );
	GXTexture2D::RemoveTexture ( parameterTexture );
}

GXVoid EMFluttershy::Render ( GXFloat deltaTime )
{
	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.SetObjectMask ( (GXUPointer)nullptr );
	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	material.SetMaximumBlurSamples ( renderer.GetMaximumMotionBlurSamples () );
	material.SetExposure ( renderer.GetMotionBlurExposure () );
	material.SetScreenResolution ( (GXUShort)coreRenderer.GetWidth (), (GXUShort)coreRenderer.GetHeight () );
	material.SetDeltaTime ( deltaTime );
	material.Bind ( mesh );
	mesh.Render ();
	material.Unbind ();

	mesh.UpdateLastFrameModelMatrix ();
}

GXVoid EMFluttershy::UpdatePose ( GXFloat deltaTime )
{
	animationSolverPlayer.Update ( deltaTime );
	skeleton.UpdatePose ( animationSolverPlayer );
	mesh.UpdatePose ( skeleton );
}

GXVoid EMFluttershy::TransformUpdated ()
{
	mesh.SetScale ( currentFrameScale );
	mesh.SetRotation ( currentFrameRotationMatrix );
	mesh.SetLocation ( currentFrameLocation );
}
