#include <GXEngine_Editor_Mobile/EMRotateGismo.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXSamplerUtils.h>


#define TEXTURE_COLOR_R			255u
#define TEXTURE_COLOR_G			255u
#define TEXTURE_COLOR_B			255u
#define TEXTURE_COLOR_A			255u

#define TEXTURE_SCALE_X			1.0f
#define TEXTURE_SCALE_Y			1.0f
#define TEXTURE_OFFSET_X		0.0f
#define TEXTURE_OFFSET_Y		0.0f

#define TEXTURE_SLOT			0u

#define ROTATE_GISMO_MESH		L"Meshes/Editor Mobile/Rotate Gismo.stm"
#define ROTATE_GISMO_TEXTURE	L"Textures/Editor Mobile/Gismo Texture.tex"

//---------------------------------------------------------------------------------------------------------------------

EMRotateGismo::EMRotateGismo ():
	isVisible ( GX_TRUE ),
	mesh ( ROTATE_GISMO_MESH )
{
	texture.LoadImage ( ROTATE_GISMO_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );

	unlitMaterial.SetColor ( TEXTURE_COLOR_R, TEXTURE_COLOR_G, TEXTURE_COLOR_B, TEXTURE_COLOR_A );
	unlitMaterial.SetTexture ( texture );
	unlitMaterial.SetTextureScale ( TEXTURE_SCALE_X, TEXTURE_SCALE_Y );
	unlitMaterial.SetTextureOffset ( TEXTURE_OFFSET_X, TEXTURE_OFFSET_Y );
}

EMRotateGismo::~EMRotateGismo ()
{
	// NOTHING
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

	EMRenderer::GetInstance ().SetObjectMask ( nullptr );

	unlitMaterial.Bind ( mesh );

	mesh.Render ();

	unlitMaterial.Unbind ();
}

GXVoid EMRotateGismo::TransformUpdated ()
{
	mesh.SetRotation ( currentFrameRotationMatrix );
	mesh.SetLocation ( currentFrameLocation );
	mesh.SetScale ( currentFrameScale );
}
