#include <GXEngine_Editor_Mobile/EMRotateGismo.h>
#include <GXEngine/GXSamplerUtils.h>


#define TEXTURE_COLOR_R		255
#define TEXTURE_COLOR_G		255
#define TEXTURE_COLOR_B		255
#define TEXTURE_COLOR_A		255

#define TEXTURE_SCALE_X		1.0f
#define TEXTURE_SCALE_Y		1.0f
#define TEXTURE_OFFSET_X	0.0f
#define TEXTURE_OFFSET_Y	0.0f

#define TEXTURE_SLOT		0


EMRotateGismo::EMRotateGismo () :
mesh ( L"3D Models/Editor Mobile/Rotate Gismo.stm" )
{
	texture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Gismo Texture.tex", GX_FALSE );

	GXGLSamplerInfo samplerInfo;
	samplerInfo.anisotropy = 1.0f;
	samplerInfo.resampling = eGXSamplerResampling::None;
	samplerInfo.wrap = GL_CLAMP_TO_EDGE;

	sampler = GXCreateSampler ( samplerInfo );

	unlitMaterial.SetColor ( TEXTURE_COLOR_R, TEXTURE_COLOR_G, TEXTURE_COLOR_B, TEXTURE_COLOR_A );
	unlitMaterial.SetTexture ( texture );
	unlitMaterial.SetTextureScale ( TEXTURE_SCALE_X, TEXTURE_SCALE_Y );
	unlitMaterial.SetTextureOffset ( TEXTURE_OFFSET_X, TEXTURE_OFFSET_Y );

	isVisible = GX_TRUE;
}

EMRotateGismo::~EMRotateGismo ()
{
	GXTexture::RemoveTexture ( texture );
	glDeleteSamplers ( 1, &sampler );
}

GXVoid EMRotateGismo::Hide ()
{
	isVisible = GX_FALSE;
}

GXVoid EMRotateGismo::Show ()
{
	isVisible = GX_TRUE;
}

GXVoid EMRotateGismo::Render ()
{
	if ( !isVisible ) return;

	glBindSampler ( TEXTURE_SLOT, sampler );
	unlitMaterial.Bind ( mesh );

	mesh.Render ();

	unlitMaterial.Unbind ();
	glBindSampler ( TEXTURE_SLOT, 0 );
}

GXVoid EMRotateGismo::TransformUpdated ()
{
	mesh.SetRotation ( rot_mat );
	mesh.SetLocation ( location );
	mesh.SetScale ( scale );
}
