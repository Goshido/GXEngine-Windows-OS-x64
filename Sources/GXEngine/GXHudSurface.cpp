// version 1.25

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
        GXMeshGeometry                  mesh;

        static GXImageRenderable*       instance;
        static GXUPointerAtomic         references;

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

GXImageRenderable*      GXImageRenderable::instance = nullptr;
GXUPointerAtomic        GXImageRenderable::references ( 0u );

GXImageRenderable& GXCALL GXImageRenderable::GetInstance ()
{
    ++references;

    if ( !instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXImageRenderable" );
        instance = new GXImageRenderable ();
    }

    return *instance;
}

GXVoid GXImageRenderable::Release ()
{
    --references;

    if ( references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXImageRenderable::Render ()
{
    mesh.Render ();
}

GXImageRenderable::GXImageRenderable ()
GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXImageRenderable" )
{
    InitGraphicResources ();
}

GXImageRenderable::~GXImageRenderable ()
{
    instance = nullptr;
}

GXVoid GXImageRenderable::InitGraphicResources ()
{
    mesh.LoadMesh ( L"Meshes/System/ScreenQuad1x1.stm" );
}

GXVoid GXImageRenderable::TransformUpdated ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXGlyphRenderable final : public GXMemoryInspector, public GXTransform, public GXRenderable
{
    private:
        GXMeshGeometry                  mesh;

        static GXGlyphRenderable*       instance;
        static GXUPointerAtomic         references;

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

GXGlyphRenderable*      GXGlyphRenderable::instance = nullptr;
GXUPointerAtomic        GXGlyphRenderable::references ( 0u );

GXGlyphRenderable& GXCALL GXGlyphRenderable::GetInstance ()
{
    ++references;

    if ( !instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXGlyphRenderable" );
        instance = new GXGlyphRenderable ();
    }

    return *instance;
}

GXVoid GXGlyphRenderable::Release ()
{
    --references;

    if ( references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXGlyphRenderable::Render ()
{
    mesh.Render ();
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

    mesh.FillVertexBuffer ( buffer, 30u * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXGlyphRenderable::GXGlyphRenderable ()
GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXGlyphRenderable" )
{
    InitGraphicResources ();
}

GXGlyphRenderable::~GXGlyphRenderable ()
{
    instance = nullptr;
}

GXVoid GXGlyphRenderable::InitGraphicResources ()
{
    GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 );
    mesh.SetTotalVertices ( 6 );
    mesh.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, static_cast<const GLvoid*> ( 0u ) );
    mesh.SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, reinterpret_cast<const GLvoid*> ( sizeof ( GXVec3 ) ) );
    mesh.SetTopology ( GL_TRIANGLES );
}

GXVoid GXGlyphRenderable::TransformUpdated ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXLineRenderable final : public GXMemoryInspector, public GXTransform, public GXRenderable
{
    private:
        GXMeshGeometry              mesh;

        static GXLineRenderable*    instance;
        static GXUPointerAtomic     references;

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

GXLineRenderable*       GXLineRenderable::instance = nullptr;
GXUPointerAtomic        GXLineRenderable::references ( 0u );

GXLineRenderable& GXCALL GXLineRenderable::GetInstance ()
{
    ++references;

    if ( !instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXLineRenderable" );
        instance = new GXLineRenderable ();
    }

    return *instance;
}

GXVoid GXLineRenderable::Release ()
{
    --references;

    if ( references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXLineRenderable::Render ()
{
    mesh.Render ();
}

GXVoid GXLineRenderable::UpdateGeometry ( const GXVec2 &start, const GXVec2 &end )
{
    static GXFloat buffer[ 6u ];
    buffer[ 0u ] = start.data[ 0u ];    buffer[ 1u ] = start.data[ 1u ];    buffer[ 2u ] = RENDER_Z;
    buffer[ 3u ] = end.data[ 0u ];      buffer[ 4u ] = end.data[ 1u ];      buffer[ 5u ] = RENDER_Z;

    mesh.FillVertexBuffer ( buffer, 6u * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXLineRenderable::GXLineRenderable ()
GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXLineRenderable" )
{
    InitGraphicResources ();
}

GXLineRenderable::~GXLineRenderable ()
{
    instance = nullptr;
}

GXVoid GXLineRenderable::InitGraphicResources ()
{
    mesh.SetTotalVertices ( 2 );
    mesh.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, sizeof ( GXVec3 ), static_cast<const GLvoid*> ( 0u ) );
    mesh.SetTopology ( GL_LINES );
}

GXVoid GXLineRenderable::TransformUpdated ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

GXHudSurface::GXHudSurface ( GXUShort imageWidth, GXUShort imageHeight )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXHudSurface" )
    image ( GXImageRenderable::GetInstance () ),
    glyph ( GXGlyphRenderable::GetInstance () ),
    line ( GXLineRenderable::GetInstance () )
{
    Resize ( imageWidth, imageHeight );
    screenQuadMesh.LoadMesh ( L"Meshes/System/ScreenQuad.stm" );

    unlitTexture2DMaterial.SetTextureScale ( 1.0f, 1.0f );
    unlitTexture2DMaterial.SetTextureOffset ( 0.0f, 0.0f );

    unlitColorMaskMaterial.SetMaskScale ( 1.0f, 1.0f );
    unlitColorMaskMaterial.SetMaskOffset ( 0.0f, 0.0f );
}

GXHudSurface::~GXHudSurface ()
{
    image.Release ();
    glyph.Release ();
    line.Release ();

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
    glDeleteFramebuffers ( 1, &fbo );
}

GXVoid GXHudSurface::Reset ()
{
    GLint oldFBO;
    glGetIntegerv ( GL_FRAMEBUFFER_BINDING, &oldFBO );

    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    static const GXFloat clearDiffuseValue[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glClearBufferfv ( GL_COLOR, 0, clearDiffuseValue );

    glBindFramebuffer ( GL_FRAMEBUFFER, static_cast<GLuint> ( oldFBO ) );
}

GXVoid GXHudSurface::Resize ( GXUShort newWidth, GXUShort newHeight )
{
    width = newWidth;
    height = newHeight;
    canvasCamera.SetProjection ( static_cast<GXFloat> ( newWidth ), static_cast<GXFloat> ( newHeight ), Z_NEAR, Z_FAR ),

    SetScale ( width * 0.5f, height * 0.5f, 1.0f );

    GLint oldFBO;
    glGetIntegerv ( GL_FRAMEBUFFER_BINDING, &oldFBO );

    glGenFramebuffers ( 1, &fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

    canvasTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
    glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvasTexture.GetTextureObject (), 0 );
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
    GXCamera::SetActiveCamera ( &canvasCamera );

    openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

    GLenum buffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers ( 1, &buffer );

    glViewport ( 0, 0, static_cast<GLsizei> ( width ), static_cast<GLsizei> ( height ) );

    image.SetScale ( imageInfo.insertWidth, imageInfo.insertHeight, 1.0f );
    image.SetLocation ( imageInfo.insertX - width * 0.5f, imageInfo.insertY - height * 0.5f, RENDER_Z );

    unlitTexture2DMaterial.SetTexture ( *imageInfo.texture );
    unlitTexture2DMaterial.SetColor ( imageInfo.color );

    unlitTexture2DMaterial.Bind ( image );

    switch ( imageInfo.overlayType )
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

    image.Render ();

    unlitTexture2DMaterial.Unbind ();

    openGLState.Restore ();

    GXCamera::SetActiveCamera ( oldCamera );
}

GXVoid GXHudSurface::AddLine ( const GXLineInfo &lineInfo )
{
    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &canvasCamera );

    openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

    glViewport ( 0, 0, static_cast<GLsizei> ( width ), static_cast<GLsizei> ( height ) );

    line.UpdateGeometry ( lineInfo.startPoint, lineInfo.endPoint );
    line.SetLocation ( -0.5f * width, -0.5f * height, RENDER_Z );

    unlitColorMaterial.SetColor ( lineInfo.color );
    unlitColorMaterial.Bind ( line );

    switch ( lineInfo.overlayType )
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

    line.Render ();

    unlitColorMaterial.Unbind ();

    openGLState.Restore ();

    GXCamera::SetActiveCamera ( oldCamera );
}

GXFloat GXHudSurface::AddText ( const GXPenInfo &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... )
{
    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &canvasCamera );

    openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );

    glViewport ( 0, 0, static_cast<GLsizei> ( width ), static_cast<GLsizei> ( height ) );

    switch ( penInfo.overlayType )
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

    GXFloat startX = -width * 0.5f;
    GXFloat startY = -height * 0.5f;

    GXGlyphInfo info;

    GXUInt prevSymbol = 0u;
    GXFloat penX = penInfo.insertX;
    GXFloat penY = penInfo.insertY;

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
                penX += static_cast<GXFloat> ( penInfo.font->GetSpaceAdvance () );
                continue;
            break;

            case '\t':
                penX += TAB_FACTOR * penInfo.font->GetSpaceAdvance ();
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

        penInfo.font->GetGlyph ( symbol, info );

        glyph.UpdateGeometry ( info.min, info.max );

        if ( prevSymbol != 0u )
            penX += static_cast<GXFloat> ( penInfo.font->GetKerning ( symbol, prevSymbol ) );

        GXFloat x = startX + penX;
        GXFloat y = startY + penY + info.offsetY;

        prevSymbol = symbol;

        glyph.SetScale ( info.width, info.height, 1.0f );
        glyph.SetLocation ( x, y, RENDER_Z );

        unlitColorMaskMaterial.SetMaskTexture ( *info.atlas );
        unlitColorMaskMaterial.SetColor ( penInfo.color );

        unlitColorMaskMaterial.Bind ( glyph );
        glyph.Render ();
        unlitColorMaskMaterial.Unbind ();

        penX += static_cast<GXFloat> ( info.advance );
    }

    if ( bufferNumSymbols )
        Free ( text );

    openGLState.Restore ();

    GXCamera::SetActiveCamera ( oldCamera );

    return penX;
}

GXUShort GXHudSurface::GetWidth () const
{
    return width;
}

GXUShort GXHudSurface::GetHeight () const
{
    return height;
}

GXVoid GXHudSurface::Render ()
{
    openGLState.Save ();

    glDisable ( GL_CULL_FACE );
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    static const GXColorRGB color ( 1.0f, 1.0f, 1.0f, 1.0f );

    unlitTexture2DMaterial.SetColor ( color );
    unlitTexture2DMaterial.SetTexture ( canvasTexture );

    unlitTexture2DMaterial.Bind ( *this );

    screenQuadMesh.Render ();

    unlitTexture2DMaterial.Unbind ();

    openGLState.Restore ();
}

GXVoid GXHudSurface::TransformUpdated ()
{
    // NOTHING
}
