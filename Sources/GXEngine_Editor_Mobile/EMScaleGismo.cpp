#include <GXEngine_Editor_Mobile/EMScaleGismo.h>


#define TEXTURE_COLOR_R     255u
#define TEXTURE_COLOR_G     255u
#define TEXTURE_COLOR_B     255u
#define TEXTURE_COLOR_A     255u

#define TEXTURE_SCALE_X     1.0f
#define TEXTURE_SCALE_Y     1.0f
#define TEXTURE_OFFSET_X    0.0f
#define TEXTURE_OFFSET_Y    0.0f

#define TEXTURE_SLOT        0u

#define SCALE_GISMO_MESH    L"Meshes/Editor Mobile/Scale Gismo.stm"

//---------------------------------------------------------------------------------------------------------------------

EMScaleGismo::EMScaleGismo () :
    _isVisible ( GX_TRUE ),
    _mesh ( SCALE_GISMO_MESH ),
    _texture ( L"Textures/Editor Mobile/Gismo Texture.tex", GX_FALSE, GX_FALSE )
{
    _unlitMaterial.SetColor ( TEXTURE_COLOR_R, TEXTURE_COLOR_G, TEXTURE_COLOR_B, TEXTURE_COLOR_A );
    _unlitMaterial.SetTexture ( _texture );
    _unlitMaterial.SetTextureScale ( TEXTURE_SCALE_X, TEXTURE_SCALE_Y );
    _unlitMaterial.SetTextureOffset ( TEXTURE_OFFSET_X, TEXTURE_OFFSET_Y );
}

EMScaleGismo::~EMScaleGismo ()
{
    // NOTHING
}

GXVoid EMScaleGismo::Hide ()
{
    _isVisible = GX_FALSE;
}

GXVoid EMScaleGismo::Show ()
{
    _isVisible = GX_TRUE;
}

GXVoid EMScaleGismo::Render ()
{
    if ( !_isVisible ) return;

    _unlitMaterial.Bind ( _mesh );
    _mesh.Render ();
    _unlitMaterial.Unbind ();
}

GXVoid EMScaleGismo::TransformUpdated ()
{
    _mesh.SetRotation ( _currentFrameRotationMatrix );
    _mesh.SetScale ( _currentFrameScale );
    _mesh.SetLocation ( _currentFrameLocation );
}
