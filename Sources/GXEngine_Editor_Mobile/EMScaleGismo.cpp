#include <GXEngine_Editor_Mobile/EMScaleGismo.h>
#include <GXEngine/GXSamplerUtils.h>


#define TEXTURE_COLOR_R		255
#define TEXTURE_COLOR_G		255
#define TEXTURE_COLOR_B		255
#define TEXTURE_COLOR_A		255

#define TEXTURE_SCALE_X		1.0f
#define TEXTURE_SCALE_Y		1.0f
#define TEXTURE_OFFSET_X	0.0f
#define TEXTURE_OFFSET_Y	0.0f

#define TEXTURE_SLOT	0


EMScaleGismo::EMScaleGismo () :
mesh ( L"3D Models/Editor Mobile/Scale Gismo.stm" )
{
	isVisible = GX_TRUE;

	texture = GXTexture2D::LoadTexture ( L"Textures/Editor Mobile/Gismo Texture.tex", GX_FALSE, GL_CLAMP_TO_EDGE );

	unlitMaterial.SetColor ( TEXTURE_COLOR_R, TEXTURE_COLOR_G, TEXTURE_COLOR_B, TEXTURE_COLOR_A );
	unlitMaterial.SetTexture ( texture );
	unlitMaterial.SetTextureScale ( TEXTURE_SCALE_X, TEXTURE_SCALE_Y );
	unlitMaterial.SetTextureOffset ( TEXTURE_OFFSET_X, TEXTURE_OFFSET_Y );
}

EMScaleGismo::~EMScaleGismo ()
{
	GXTexture2D::RemoveTexture ( texture );
}

GXVoid EMScaleGismo::Hide ()
{
	isVisible = GX_FALSE;
}

GXVoid EMScaleGismo::Show ()
{
	isVisible = GX_TRUE;
}

GXVoid EMScaleGismo::Render ()
{
	if ( !isVisible ) return;

	unlitMaterial.Bind ( mesh );

	mesh.Render ();

	unlitMaterial.Unbind ();
}

GXVoid EMScaleGismo::TransformUpdated ()
{
	mesh.SetRotation ( currentFrameRotationMatrix );
	mesh.SetScale ( currentFrameScale );
	mesh.SetLocation ( currentFrameLocation );
}
