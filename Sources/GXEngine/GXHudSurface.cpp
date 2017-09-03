//version 1.18

#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXStrings.h>


#define BYTES_PER_PIXEL 4
#define TAB_FACTOR		4.0f
#define Z_NEAR			0.0f
#define Z_FAR			20.0f
#define RENDER_Z		7.0f
#define TEXTURE_SLOT	0


class GXImageRenderable : public GXTransform, public GXRenderable
{
	private:
		GXMeshGeometry		mesh;
	
	public:
		GXImageRenderable ();
		~GXImageRenderable () override;

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid TransformUpdated () override;
};

GXImageRenderable::GXImageRenderable ()
{
	InitGraphicResources ();
}

GXImageRenderable::~GXImageRenderable ()
{
	GXMeshGeometry::RemoveMeshGeometry ( mesh );
}

GXVoid GXImageRenderable::Render ()
{
	mesh.Render ();
}

GXVoid GXImageRenderable::InitGraphicResources ()
{
	mesh = GXMeshGeometry::LoadFromStm ( L"3D Models/System/ScreenQuad1x1.stm" );
}

GXVoid GXImageRenderable::TransformUpdated ()
{
	//NOTHING
}

//-----------------------------------------------------------------------------------------

class GXGlyphRenderable : public GXTransform, public GXRenderable
{
	private:
		GXMeshGeometry		mesh;

	public:
		GXGlyphRenderable ();
		~GXGlyphRenderable () override;

		GXVoid Render () override;

		GXVoid UpdateGeometry ( const GXVec2 &min, const GXVec2 &max );

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid TransformUpdated () override;
};

GXGlyphRenderable::GXGlyphRenderable ()
{
	InitGraphicResources ();
}

GXGlyphRenderable::~GXGlyphRenderable ()
{
	//NOTHING
}

GXVoid GXGlyphRenderable::Render ()
{
	mesh.Render ();
}

GXVoid GXGlyphRenderable::UpdateGeometry ( const GXVec2 &min, const GXVec2 &max )
{
	static GXFloat buffer[ 30 ];
	buffer[ 0 ] = 0.0f;		buffer[ 1 ] = 1.0f;		buffer[ 2 ] = 0.0f;
	buffer[ 3 ] = min.x;	buffer[ 4 ] = max.y;

	buffer[ 5 ] = 1.0f;		buffer[ 6 ] = 0.0f;		buffer[ 7 ] = 0.0f;
	buffer[ 8 ] = max.x;	buffer[ 9 ] = min.y;

	buffer[ 10 ] = 0.0f;	buffer[ 11 ] = 0.0f;	buffer[ 12 ] = 0.0f;
	buffer[ 13 ] = min.x;	buffer[ 14 ] = min.y;

	buffer[ 15 ] = 0.0f;	buffer[ 16 ] = 1.0f;	buffer[ 17 ] = 0.0f;
	buffer[ 18 ] = min.x;	buffer[ 19 ] = max.y;

	buffer[ 20 ] = 1.0f;	buffer[ 21 ] = 1.0f;	buffer[ 22 ] = 0.0f;
	buffer[ 23 ] = max.x;	buffer[ 24 ] = max.y;

	buffer[ 25 ] = 1.0f;	buffer[ 26 ] = 0.0f;	buffer[ 27 ] = 0.0f;
	buffer[ 28 ] = max.x;	buffer[ 29 ] = min.y;

	mesh.FillVertexBuffer ( buffer, 30 * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXVoid GXGlyphRenderable::InitGraphicResources ()
{
	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 );
	mesh.SetTotalVertices ( 6 );
	mesh.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, (const GLvoid*)0 );
	mesh.SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, ( const GLvoid* )sizeof ( GXVec3 ) );
	mesh.SetTopology ( GL_TRIANGLES );
}

GXVoid GXGlyphRenderable::TransformUpdated ()
{
	//NOTHING
}

//-----------------------------------------------------------------------------------------

class GXLineRenderable : public GXTransform, public GXRenderable
{
	private:
		GXMeshGeometry		mesh;

	public:
		GXLineRenderable ();
		~GXLineRenderable () override;

		GXVoid Render () override;

		GXVoid UpdateGeometry ( const GXVec2 &start, const GXVec2 &end );

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid TransformUpdated () override;
};

GXLineRenderable::GXLineRenderable ()
{
	InitGraphicResources ();
}

GXLineRenderable::~GXLineRenderable ()
{
	//NOTHING
}

GXVoid GXLineRenderable::Render ()
{
	mesh.Render ();
}

GXVoid GXLineRenderable::UpdateGeometry ( const GXVec2 &start, const GXVec2 &end )
{
	static GXFloat buffer[ 6 ];
	buffer[ 0 ] = start.x;	buffer[ 1 ] = start.y;	buffer[ 2 ] = RENDER_Z;
	buffer[ 3 ] = end.x;	buffer[ 4 ] = end.y;	buffer[ 5 ] = RENDER_Z;

	mesh.FillVertexBuffer ( buffer, 6 * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXVoid GXLineRenderable::InitGraphicResources ()
{
	mesh.SetTotalVertices ( 2 );
	mesh.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, sizeof ( GXVec3 ), (const GLvoid*)0 );
	mesh.SetTopology ( GL_LINES );
}

GXVoid GXLineRenderable::TransformUpdated ()
{
	//NOTHING
}

//-----------------------------------------------------------------------------------------

GXHudSurface::GXHudSurface ( GXUShort width, GXUShort height ) :
canvasCamera ( (GXFloat)width, (GXFloat)height, Z_NEAR, Z_FAR )
{
	this->width = width;
	this->height = height;

	GXUInt size = (GXUInt)( ( width * height ) * BYTES_PER_PIXEL );
	GXUByte* data = (GXUByte*)malloc ( size );
	memset ( data, 0, size );

	canvasTexture.InitResources ( width, height, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
	canvasTexture.FillWholePixelData ( data );
	free ( data );

	SetScale ( width * 0.5f, height * 0.5f, 1.0f );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvasTexture.GetTextureObject (), 0 );
	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXHudSurface::GXHudSurfaceExt::Error - Что-то с fbo. Ошибка 0x%04x\n", status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	screenQuadMesh = GXMeshGeometry::LoadFromStm ( L"3D Models/System/ScreenQuad.stm" );

	image = new GXImageRenderable ();
	glyph = new GXGlyphRenderable ();

	line = new GXLineRenderable ();
	line->SetLocation ( -0.5f * width, -0.5f * height, RENDER_Z );

	unlitTexture2DMaterial.SetTextureScale ( 1.0f, 1.0f );
	unlitTexture2DMaterial.SetTextureOffset ( 0.0f, 0.0f );

	unlitColorMaskMaterial.SetMaskScale ( 1.0f, 1.0f );
	unlitColorMaskMaterial.SetMaskOffset ( 0.0f, 0.0f );
}

GXHudSurface::~GXHudSurface ()
{
	delete image;
	delete glyph;
	delete line;

	GXMeshGeometry::RemoveMeshGeometry ( screenQuadMesh );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );
}

GXVoid GXHudSurface::Reset ()
{
	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	static const GXFloat clearDiffuseValue[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearBufferfv ( GL_COLOR, 0, clearDiffuseValue );

	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
}

GXVoid GXHudSurface::AddImage ( const GXImageInfo &imageInfo )
{
	GXCamera* oldCamera = GXCamera::GetActiveCamera ();
	GXCamera::SetActiveCamera ( &canvasCamera );

	openGLState.Save ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );

	image->SetScale ( imageInfo.insertWidth, imageInfo.insertHeight, 1.0f );
	image->SetLocation ( imageInfo.insertX - width * 0.5f, imageInfo.insertY - height * 0.5f, RENDER_Z );

	unlitTexture2DMaterial.SetTexture ( *imageInfo.texture );
	unlitTexture2DMaterial.SetColor ( imageInfo.color );

	unlitTexture2DMaterial.Bind ( *image );

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

	image->Render ();

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

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );

	line->UpdateGeometry ( lineInfo.startPoint, lineInfo.endPoint );

	unlitColorMaterial.SetColor ( lineInfo.color );
	unlitColorMaterial.Bind ( *line );

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

	line->Render ();

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

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );

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

	GXUInt prevSymbol = 0;
	GXFloat penX = penInfo.insertX;
	GXFloat penY = penInfo.insertY;

	GXWChar* text = nullptr;

	if ( bufferNumSymbols )
	{
		GXWChar* temp = (GXWChar*)malloc ( bufferNumSymbols * sizeof ( GXWChar ) );

		va_list ap;
		va_start ( ap, format );
		vswprintf_s ( temp, bufferNumSymbols, format, ap );
		va_end ( ap );

		text = temp;
	}
	else
		text = (GXWChar*)format;

	GXUInt len = GXWcslen ( text );

	for ( GXUInt i = 0; i < len; i++ )
	{
		GXUInt symbol = (GXUInt)text[ i ];

		switch ( symbol )
		{
			case ' ':
				penX += (GXFloat)penInfo.font->GetSpaceAdvance ();
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
		}

		penInfo.font->GetGlyph ( symbol, info );

		glyph->UpdateGeometry ( info.min, info.max );

		if ( prevSymbol != 0 )
			penX += (GXFloat)penInfo.font->GetKerning ( symbol, prevSymbol );

		GXFloat x = startX + penX;
		GXFloat y = startY + penY + info.offsetY;

		prevSymbol = symbol;

		glyph->SetScale ( info.width, info.height, 1.0f );
		glyph->SetLocation ( x, y, RENDER_Z );

		unlitColorMaskMaterial.SetMaskTexture ( *info.atlas );
		unlitColorMaskMaterial.SetColor ( penInfo.color );

		unlitColorMaskMaterial.Bind ( *glyph );
		glyph->Render ();
		unlitColorMaskMaterial.Unbind ();

		penX += (GXFloat)info.advance;
	}

	if ( bufferNumSymbols )
		free ( text );

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

	static const GXVec4 color ( 1.0f, 1.0f, 1.0f, 1.0f );

	unlitTexture2DMaterial.SetColor ( color );
	unlitTexture2DMaterial.SetTexture ( canvasTexture );

	unlitTexture2DMaterial.Bind ( *this );

	screenQuadMesh.Render ();

	unlitTexture2DMaterial.Unbind ();

	openGLState.Restore ();
}

GXVoid GXHudSurface::TransformUpdated ()
{
	//NOTHING
}
