// version 1.26

#include <GXEngine/GXHudSurface.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUPointerAtomic.h>


#define BYTES_PER_PIXEL     4u
#define TAB_FACTOR          4.0f
#define Z_NEAR              0.0f
#define Z_FAR               20.0f
#define RENDER_Z            7.0f

//---------------------------------------------------------------------------------------------------------------------

class GXImageRenderable final : public GXMemoryInspector, public GXTransform, public GXRenderable
{
    private:
        GXMeshGeometry                  _mesh;

        static GXImageRenderable*       _instance;
        static GXUPointerAtomic         _references;

    public:
        static GXImageRenderable& GXCALL GetInstance ();
        GXVoid Release ();

        GXVoid Render () override;

    private:
        GXImageRenderable ();
        ~GXImageRenderable () override;

        GXVoid InitGraphicResources () override;
        GXVoid TransformUpdated () override;

        GXImageRenderable ( const GXImageRenderable &other ) = delete;
        GXImageRenderable& operator = ( const GXImageRenderable &other ) = delete;
};

GXImageRenderable*      GXImageRenderable::_instance = nullptr;
GXUPointerAtomic        GXImageRenderable::_references ( 0u );

GXImageRenderable& GXCALL GXImageRenderable::GetInstance ()
{
    ++_references;

    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXImageRenderable" );
        _instance = new GXImageRenderable ();
    }

    return *_instance;
}

GXVoid GXImageRenderable::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXImageRenderable::Render ()
{
    _mesh.Render ();
}

GXImageRenderable::GXImageRenderable ()
GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXImageRenderable" )
{
    InitGraphicResources ();
}

GXImageRenderable::~GXImageRenderable ()
{
    _instance = nullptr;
}

GXVoid GXImageRenderable::InitGraphicResources ()
{
    _mesh.LoadMesh ( L"Meshes/System/ScreenQuad1x1.stm" );
}

GXVoid GXImageRenderable::TransformUpdated ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXGlyphRenderable final : public GXMemoryInspector, public GXTransform, public GXRenderable
{
    private:
        GXMeshGeometry                  _mesh;

        static GXGlyphRenderable*       _instance;
        static GXUPointerAtomic         _references;

    public:
        static GXGlyphRenderable& GXCALL GetInstance ();
        GXVoid Release ();

        GXVoid Render () override;

        GXVoid UpdateGeometry ( const GXVec2 &min, const GXVec2 &max );

    private:
        GXGlyphRenderable ();
        ~GXGlyphRenderable () override;

        GXVoid InitGraphicResources () override;
        GXVoid TransformUpdated () override;

        GXGlyphRenderable ( const GXGlyphRenderable &other ) = delete;
        GXGlyphRenderable& operator = ( const GXGlyphRenderable &other ) = delete;
};

GXGlyphRenderable*      GXGlyphRenderable::_instance = nullptr;
GXUPointerAtomic        GXGlyphRenderable::_references ( 0u );

GXGlyphRenderable& GXCALL GXGlyphRenderable::GetInstance ()
{
    ++_references;

    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXGlyphRenderable" );
        _instance = new GXGlyphRenderable ();
    }

    return *_instance;
}

GXVoid GXGlyphRenderable::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXGlyphRenderable::Render ()
{
    _mesh.Render ();
}

GXVoid GXGlyphRenderable::UpdateGeometry ( const GXVec2 &min, const GXVec2 &max )
{
    static GXFloat buffer[ 30u ];

    buffer[ 0u ] = 0.0f;                buffer[ 1u ] = 1.0f;                buffer[ 2u ] = 0.0f;
    buffer[ 3u ] = min.data[ 0u ];      buffer[ 4u ] = max.data[ 1u ];

    buffer[ 5u ] = 1.0f;                buffer[ 6u ] = 0.0f;                buffer[ 7u ] = 0.0f;
    buffer[ 8u ] = max.data[ 0u ];      buffer[ 9u ] = min.data[ 1u ];

    buffer[ 10u ] = 0.0f;               buffer[ 11u ] = 0.0f;               buffer[ 12u ] = 0.0f;
    buffer[ 13u ] = min.data[ 0u ];     buffer[ 14u ] = min.data[ 1u ];

    buffer[ 15u ] = 0.0f;               buffer[ 16u ] = 1.0f;               buffer[ 17u ] = 0.0f;
    buffer[ 18u ] = min.data[ 0u ];     buffer[ 19u ] = max.data[ 1u ];

    buffer[ 20u ] = 1.0f;               buffer[ 21u ] = 1.0f;               buffer[ 22u ] = 0.0f;
    buffer[ 23u ] = max.data[ 0u ];     buffer[ 24u ] = max.data[ 1u ];

    buffer[ 25u ] = 1.0f;               buffer[ 26u ] = 0.0f;               buffer[ 27u ] = 0.0f;
    buffer[ 28u ] = max.data[ 0u ];     buffer[ 29u ] = min.data[ 1u ];

    _mesh.FillVertexBuffer ( buffer, 30u * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXGlyphRenderable::GXGlyphRenderable ()
GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXGlyphRenderable" )
{
    InitGraphicResources ();
}

GXGlyphRenderable::~GXGlyphRenderable ()
{
    _instance = nullptr;
}

GXVoid GXGlyphRenderable::InitGraphicResources ()
{
    GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 );
    _mesh.SetTotalVertices ( 6 );
    _mesh.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, static_cast<const GLvoid*> ( 0u ) );
    _mesh.SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, reinterpret_cast<const GLvoid*> ( sizeof ( GXVec3 ) ) );
    _mesh.SetTopology ( GL_TRIANGLES );
}

GXVoid GXGlyphRenderable::TransformUpdated ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXLineRenderable final : public GXMemoryInspector, public GXTransform, public GXRenderable
{
    private:
        GXMeshGeometry              _mesh;

        static GXLineRenderable*    _instance;
        static GXUPointerAtomic     _references;

    public:
        static GXLineRenderable& GXCALL GetInstance ();
        GXVoid Release ();

        GXVoid Render () override;

        GXVoid UpdateGeometry ( const GXVec2 &start, const GXVec2 &end );

    private:
        GXLineRenderable ();
        ~GXLineRenderable () override;

        GXVoid InitGraphicResources () override;
        GXVoid TransformUpdated () override;

        GXLineRenderable ( const GXLineRenderable &other ) = delete;
        GXLineRenderable& operator = ( const GXLineRenderable &other ) = delete;
};

GXLineRenderable*       GXLineRenderable::_instance = nullptr;
GXUPointerAtomic        GXLineRenderable::_references ( 0u );

GXLineRenderable& GXCALL GXLineRenderable::GetInstance ()
{
    ++_references;

    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXLineRenderable" );
        _instance = new GXLineRenderable ();
    }

    return *_instance;
}

GXVoid GXLineRenderable::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXLineRenderable::Render ()
{
    _mesh.Render ();
}

GXVoid GXLineRenderable::UpdateGeometry ( const GXVec2 &start, const GXVec2 &end )
{
    static GXFloat buffer[ 6u ];
    buffer[ 0u ] = start.data[ 0u ];    buffer[ 1u ] = start.data[ 1u ];    buffer[ 2u ] = RENDER_Z;
    buffer[ 3u ] = end.data[ 0u ];      buffer[ 4u ] = end.data[ 1u ];      buffer[ 5u ] = RENDER_Z;

    _mesh.FillVertexBuffer ( buffer, 6u * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXLineRenderable::GXLineRenderable ()
GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXLineRenderable" )
{
    InitGraphicResources ();
}

GXLineRenderable::~GXLineRenderable ()
{
    _instance = nullptr;
}

GXVoid GXLineRenderable::InitGraphicResources ()
{
    _mesh.SetTotalVertices ( 2 );
    _mesh.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, sizeof ( GXVec3 ), static_cast<const GLvoid*> ( 0u ) );
    _mesh.SetTopology ( GL_LINES );
}

GXVoid GXLineRenderable::TransformUpdated ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

GXHudSurface::GXHudSurface ( GXUShort width, GXUShort height )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXHudSurface" )
    _image ( GXImageRenderable::GetInstance () ),
    _glyph ( GXGlyphRenderable::GetInstance () ),
    _line ( GXLineRenderable::GetInstance () )
{
    Resize ( width, height );
    _screenQuadMesh.LoadMesh ( L"Meshes/System/ScreenQuad.stm" );

    _unlitTexture2DMaterial.SetTextureScale ( 1.0f, 1.0f );
    _unlitTexture2DMaterial.SetTextureOffset ( 0.0f, 0.0f );

    _unlitColorMaskMaterial.SetMaskScale ( 1.0f, 1.0f );
    _unlitColorMaskMaterial.SetMaskOffset ( 0.0f, 0.0f );
}

GXHudSurface::~GXHudSurface ()
{
    _image.Release ();
    _glyph.Release ();
    _line.Release ();

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
    glDeleteFramebuffers ( 1, &_fbo );
}

GXVoid GXHudSurface::Reset ()
{
    GLint oldFBO;
    glGetIntegerv ( GL_FRAMEBUFFER_BINDING, &oldFBO );

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    static const GXFloat clearDiffuseValue[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glClearBufferfv ( GL_COLOR, 0, clearDiffuseValue );

    glBindFramebuffer ( GL_FRAMEBUFFER, static_cast<GLuint> ( oldFBO ) );
}

GXVoid GXHudSurface::Resize ( GXUShort width, GXUShort height )
{
    _width = width;
    _height = height;
    _canvasCamera.SetProjection ( static_cast<GXFloat> ( width ), static_cast<GXFloat> ( height ), Z_NEAR, Z_FAR ),

    SetScale ( _width * 0.5f, _height * 0.5f, 1.0f );

    GLint oldFBO;
    glGetIntegerv ( GL_FRAMEBUFFER_BINDING, &oldFBO );

    glGenFramebuffers ( 1, &_fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );

    _canvasTexture.InitResources ( _width, _height, GL_RGBA8, GX_FALSE );
    glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _canvasTexture.GetTextureObject (), 0 );
    GLenum buffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers ( 1, &buffer );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "GXHudSurface::Resize::Error - Что-то с fbo. Ошибка 0x%04x\n", status );

    glBindFramebuffer ( GL_FRAMEBUFFER, static_cast<GLuint> ( oldFBO ) );

    Reset ();
}

GXVoid GXHudSurface::AddImage ( const GXImageInfo &imageInfo )
{
    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &_canvasCamera );

    _openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );

    GLenum buffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers ( 1, &buffer );

    glViewport ( 0, 0, static_cast<GLsizei> ( _width ), static_cast<GLsizei> ( _height ) );

    _image.SetScale ( imageInfo._insertWidth, imageInfo._insertHeight, 1.0f );
    _image.SetLocation ( imageInfo._insertX - _width * 0.5f, imageInfo._insertY - _height * 0.5f, RENDER_Z );

    _unlitTexture2DMaterial.SetTexture ( *imageInfo._texture );
    _unlitTexture2DMaterial.SetColor ( imageInfo._color );

    _unlitTexture2DMaterial.Bind ( _image );

    switch ( imageInfo._overlayType )
    {
        case eGXImageOverlayType::AlphaTransparency:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;

        case eGXImageOverlayType::AlphaTransparencyPreserveAlpha:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;

        case eGXImageOverlayType::AlphaAdd:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_ONE, GL_ONE );
        break;

        case eGXImageOverlayType::AlphaMultiply:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
        break;

        case eGXImageOverlayType::SimpleReplace:
        default:
            glDisable ( GL_BLEND );
        break;
    }

    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );

    _image.Render ();

    _unlitTexture2DMaterial.Unbind ();

    _openGLState.Restore ();

    GXCamera::SetActiveCamera ( oldCamera );
}

GXVoid GXHudSurface::AddLine ( const GXLineInfo &lineInfo )
{
    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &_canvasCamera );

    _openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );

    glViewport ( 0, 0, static_cast<GLsizei> ( _width ), static_cast<GLsizei> ( _height ) );

    _line.UpdateGeometry ( lineInfo._startPoint, lineInfo._endPoint );
    _line.SetLocation ( -0.5f * _width, -0.5f * _height, RENDER_Z );

    _unlitColorMaterial.SetColor ( lineInfo._color );
    _unlitColorMaterial.Bind ( _line );

    switch ( lineInfo._overlayType )
    {
        case eGXImageOverlayType::AlphaTransparency:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;

        case eGXImageOverlayType::AlphaTransparencyPreserveAlpha:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;

        case eGXImageOverlayType::AlphaAdd:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_ONE, GL_ONE );
        break;

        case eGXImageOverlayType::AlphaMultiply:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
        break;

        case eGXImageOverlayType::SimpleReplace:
        default:
            glDisable ( GL_BLEND );
        break;
    }

    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );

    _line.Render ();

    _unlitColorMaterial.Unbind ();

    _openGLState.Restore ();

    GXCamera::SetActiveCamera ( oldCamera );
}

GXFloat GXHudSurface::AddText ( const GXPenInfo &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... )
{
    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &_canvasCamera );

    _openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );

    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );

    glViewport ( 0, 0, static_cast<GLsizei> ( _width ), static_cast<GLsizei> ( _height ) );

    switch ( penInfo._overlayType )
    {
        case eGXImageOverlayType::AlphaTransparency:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;

        case eGXImageOverlayType::AlphaTransparencyPreserveAlpha:
            glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;

        case eGXImageOverlayType::AlphaAdd:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_ONE, GL_ONE );
        break;

        case eGXImageOverlayType::AlphaMultiply:
            glEnable ( GL_BLEND );
            glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
        break;

        case eGXImageOverlayType::SimpleReplace:
        default:
            glDisable ( GL_BLEND );
        break;
    }

    GXFloat startX = -_width * 0.5f;
    GXFloat startY = -_height * 0.5f;

    GXGlyphInfo info;

    GXUInt prevSymbol = 0u;
    GXFloat penX = penInfo._insertX;
    GXFloat penY = penInfo._insertY;

    GXWChar* text = nullptr;

    if ( bufferNumSymbols )
    {
        GXWChar* temp = static_cast<GXWChar*> ( Malloc ( bufferNumSymbols * sizeof ( GXWChar ) ) );

        va_list ap;
        va_start ( ap, format );
        vswprintf_s ( temp, bufferNumSymbols, format, ap );
        va_end ( ap );

        text = temp;
    }
    else
        text = const_cast<GXWChar*> ( format );

    GXUPointer len = GXWcslen ( text );

    for ( GXUPointer i = 0u; i < len; ++i )
    {
        GXUInt symbol = static_cast<GXUInt> ( text[ i ] );

        switch ( symbol )
        {
            case ' ':
                penX += static_cast<GXFloat> ( penInfo._font->GetSpaceAdvance () );
                continue;
            break;

            case '\t':
                penX += TAB_FACTOR * penInfo._font->GetSpaceAdvance ();
                continue;
            break;

            case '\n':
            case '\r':
                continue;
            break;

            default:
                // NOTHING
            break;
        }

        penInfo._font->GetGlyph ( symbol, info );

        _glyph.UpdateGeometry ( info._min, info._max );

        if ( prevSymbol != 0u )
            penX += static_cast<GXFloat> ( penInfo._font->GetKerning ( symbol, prevSymbol ) );

        GXFloat x = startX + penX;
        GXFloat y = startY + penY + info._offsetY;

        prevSymbol = symbol;

        _glyph.SetScale ( info._width, info._height, 1.0f );
        _glyph.SetLocation ( x, y, RENDER_Z );

        _unlitColorMaskMaterial.SetMaskTexture ( *info._atlas );
        _unlitColorMaskMaterial.SetColor ( penInfo._color );

        _unlitColorMaskMaterial.Bind ( _glyph );
        _glyph.Render ();
        _unlitColorMaskMaterial.Unbind ();

        penX += static_cast<GXFloat> ( info._advance );
    }

    if ( bufferNumSymbols )
        Free ( text );

    _openGLState.Restore ();

    GXCamera::SetActiveCamera ( oldCamera );

    return penX;
}

GXUShort GXHudSurface::GetWidth () const
{
    return _width;
}

GXUShort GXHudSurface::GetHeight () const
{
    return _height;
}

GXVoid GXHudSurface::Render ()
{
    _openGLState.Save ();

    glDisable ( GL_CULL_FACE );
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    static const GXColorRGB color ( 1.0f, 1.0f, 1.0f, 1.0f );

    _unlitTexture2DMaterial.SetColor ( color );
    _unlitTexture2DMaterial.SetTexture ( _canvasTexture );

    _unlitTexture2DMaterial.Bind ( *this );

    _screenQuadMesh.Render ();

    _unlitTexture2DMaterial.Unbind ();

    _openGLState.Restore ();
}

GXVoid GXHudSurface::TransformUpdated ()
{
    // NOTHING
}
