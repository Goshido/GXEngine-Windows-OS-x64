#include <GXEngine_Editor_Mobile/EMFluttershy.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>


#define SOLVER_ID		0


EMFluttershy::EMFluttershy () :
mesh ( L"3D Models/Editor Mobile/Fluttershy.skm" ), animationSolverPlayer ( SOLVER_ID )
{
	GXLoadNativeAnimation ( L"Animations/Editor Mobile/Battle.ani", animationInfo );
	skeleton.LoadFromSkm ( L"3D Models/Editor Mobile/Fluttershy.skm" );
	animationSolverPlayer.SetAnimationSequence ( &animationInfo );

	diffuseTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Fluttershy_Diffuse.tga", GX_TRUE, GL_REPEAT );
	//normalTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Brick_Wall_Normals.tga", GX_TRUE, GL_REPEAT );
	normalTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Normals.tex", GX_FALSE, GL_CLAMP_TO_EDGE );
	specularTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Specular.tex", GX_FALSE, GL_CLAMP_TO_EDGE );
	emissionTexture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Default Emission.tex", GX_FALSE, GL_CLAMP_TO_EDGE );

	EMRenderer* renderer = EMRenderer::GetInstance ();

	material.SetDiffuseTexture ( diffuseTexture );
	material.SetNormalTexture ( normalTexture );
	material.SetSpecularTexture ( specularTexture );
	material.SetEmissionTexture ( emissionTexture );
	material.SetMaxBlurSamples ( renderer->GetMaxBlurSamples () );
	material.SetExplosureTime ( renderer->GetExplosureTime () );
}

EMFluttershy::~EMFluttershy ()
{
	animationInfo.Cleanup ();

	GXTexture::RemoveTexture ( diffuseTexture );
	GXTexture::RemoveTexture ( normalTexture );
	GXTexture::RemoveTexture ( specularTexture );
	GXTexture::RemoveTexture ( emissionTexture );
}

GXVoid EMFluttershy::Render ( GXFloat deltaTime )
{
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
	mesh.SetScale ( currentScale );
	mesh.SetRotation ( currentRotationMatrix );
	mesh.SetLocation ( currentLocation );
}
