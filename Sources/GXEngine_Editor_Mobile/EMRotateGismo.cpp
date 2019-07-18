#include <GXEngine_Editor_Mobile/EMRotateGismo.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>


#define TEXTURE_COLOR_R         255u
#define TEXTURE_COLOR_G         255u
#define TEXTURE_COLOR_B         255u
#define TEXTURE_COLOR_A         255u

#define TEXTURE_SCALE_X         1.0f
#define TEXTURE_SCALE_Y         1.0f
#define TEXTURE_OFFSET_X        0.0f
#define TEXTURE_OFFSET_Y        0.0f

#define TEXTURE_SLOT            0u

#define ROTATE_GISMO_MESH       L"Meshes/Editor Mobile/Rotate Gismo.stm"
#define ROTATE_GISMO_TEXTURE    L"Textures/Editor Mobile/Gismo Texture.tex"

//---------------------------------------------------------------------------------------------------------------------

EMRotateGismo::EMRotateGismo ():
    _isVisible ( GX_TRUE ),
    _mesh ( ROTATE_GISMO_MESH )
{
    _texture.LoadImage ( ROTATE_GISMO_TEXTURE, GX_FALSE, GX_FALSE );

    _unlitMaterial.SetColor ( TEXTURE_COLOR_R, TEXTURE_COLOR_G, TEXTURE_COLOR_B, TEXTURE_COLOR_A );
    _unlitMaterial.SetTexture ( _texture );
    _unlitMaterial.SetTextureScale ( TEXTURE_SCALE_X, TEXTURE_SCALE_Y );
    _unlitMaterial.SetTextureOffset ( TEXTURE_OFFSET_X, TEXTURE_OFFSET_Y );
}

EMRotateGismo::~EMRotateGismo ()
{
    // NOTHING
}

GXVoid EMRotateGismo::Hide ()
{
    _isVisible = GX_FALSE;
}

GXVoid EMRotateGismo::Show ()
{
    _isVisible = GX_TRUE;
}

GXVoid EMRotateGismo::Render ()
{
    if ( !_isVisible ) return;

    EMRenderer::GetInstance ().SetObjectMask ( nullptr );

    _unlitMaterial.Bind ( _mesh );

    _mesh.Render ();

    _unlitMaterial.Unbind ();
}

GXVoid EMRotateGismo::TransformUpdated ()
{
    _mesh.SetRotation ( _currentFrameRotationMatrix );
    _mesh.SetLocation ( _currentFrameLocation );
    _mesh.SetScale ( _currentFrameScale );
}
