//version 1.18

#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXStrings.h>
#include <cstdio>


#define BYTES_PER_PIXEL 4
#define TAB_FACTOR		4.0f
#define Z_NEAR			0.0f
#define Z_FAR			20.0f
#define RENDER_Z		7.0f
#define TEXTURE_SLOT	0


GXHudSurface::GXHudSurface ( GXUShort width, GXUShort height )
{
	this->width = width;
	this->height = height;

	GXFloat aspect = width / (GXFloat)height;
	GXSetMat4Ortho ( projectionMatrix, (GXFloat)width, (GXFloat)height, Z_NEAR, Z_FAR );

	InitGraphicResources ();
}

GXHudSurface::~GXHudSurface ()
{
	GXMeshGeometry::RemoveMeshGeometry ( screenQuad );
	GXMeshGeometry::RemoveMeshGeometry ( surfaceMesh );

	GXShaderProgram::RemoveShaderProgram ( imageShaderProgram );
	GXShaderProgram::RemoveShaderProgram ( glyphShaderProgram );
	GXShaderProgram::RemoveShaderProgram ( lineShaderProgram );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	glDeleteSamplers ( 1, &sampler );
}

GXVoid GXHudSurface::Reset ()
{
	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClear ( GL_COLOR_BUFFER_BIT );

	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
}

GXVoid GXHudSurface::AddImage ( const GXImageInfo &imageInfo )
{
	GXMat4 imageModelMatrix;
	GXMat4 imageModelProjectionMatrix;

	GXSetMat4Scale ( imageModelMatrix, imageInfo.insertWidth, imageInfo.insertHeight, 1.0f );
	imageModelMatrix.m41 = imageInfo.insertX - width * 0.5f;
	imageModelMatrix.m42 = imageInfo.insertY - height * 0.5f;
	imageModelMatrix.m43 = RENDER_Z;

	GXMulMat4Mat4 ( imageModelProjectionMatrix, imageModelMatrix, projectionMatrix );

	GLint oldVP[ 4 ];
	glGetIntegerv ( GL_VIEWPORT, oldVP );

	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );

	glUseProgram ( imageShaderProgram.GetProgram () );

	imageInfo.texture->Bind ( 0 );
	glBindSampler ( 0, sampler );

	glUniformMatrix4fv ( imageModelViewProjectionMatrixLocation, 1, GL_FALSE, imageModelProjectionMatrix.arr );
	glUniform4fv ( imageColorLocation, 1, imageInfo.color.arr );

	GLboolean isBlend;
	glGetBooleanv ( GL_BLEND, &isBlend );

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

	screenQuad.Render ();

	if ( imageInfo.overlayType == eGXImageOverlayType::AlphaTransparencyPreserveAlpha )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	if ( isBlend )
		glEnable ( GL_BLEND );
	else
		glDisable ( GL_BLEND );

	imageInfo.texture->Unbind ();
	glBindSampler ( 0, 0 );

	glUseProgram ( 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
	glViewport ( oldVP[ 0 ], oldVP[ 1 ], oldVP[ 2 ], oldVP[ 3 ] );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
}

GXVoid GXHudSurface::AddLine ( const GXLineInfo &lineInfo )
{
	GLint oldVP[ 4 ];
	glGetIntegerv ( GL_VIEWPORT, oldVP );

	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );
	glUseProgram ( lineShaderProgram.GetProgram () );

	UpdateLineGeometry ( lineInfo.startPoint, lineInfo.endPoint );

	GXMat4 lineModelMatrix;
	GXSetMat4Translation ( lineModelMatrix, -0.5f * width, -0.5f * height, 0.0f );

	GXMat4 lineModelProjectionMatrix;
	GXMulMat4Mat4 ( lineModelProjectionMatrix, lineModelMatrix, projectionMatrix );

	glUniform4fv ( lineColorLocation, 1, lineInfo.color.arr );
	glUniformMatrix4fv ( lineModelViewProjectionMatrixLocation, 1, GX_FALSE, lineModelProjectionMatrix.arr );

	GLboolean isBlend;
	glGetBooleanv ( GL_BLEND, &isBlend );

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

	lineMesh.Render ();

	if ( lineInfo.overlayType == eGXImageOverlayType::AlphaTransparencyPreserveAlpha )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
	glViewport ( oldVP[ 0 ], oldVP[ 1 ], oldVP[ 2 ], oldVP[ 3 ] );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );
}

GXFloat GXHudSurface::AddText ( const GXPenInfo &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... )
{
	GLint oldVP[ 4 ];
	glGetIntegerv ( GL_VIEWPORT, oldVP );

	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );

	glViewport ( 0, 0, (GLsizei)width, (GLsizei)height );
	glUseProgram ( glyphShaderProgram.GetProgram () );

	glBindSampler ( TEXTURE_SLOT, sampler );

	GLboolean isBlend;
	glGetBooleanv ( GL_BLEND, &isBlend );

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

	GXMat4 mvp;
	GXMat4 mv;

	GXFloat startX = -width * 0.5f;
	GXFloat startY = -height * 0.5f;

	GXGlyphInfo info;

	GXUInt prevSymbol = 0;
	GXFloat penX = penInfo.insertX;
	GXFloat penY = penInfo.insertY;

	GXWChar* text = 0;

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

		UpdateGlyphGeometry ( info.min, info.max );

		if ( prevSymbol != 0 )
			penX += (GXFloat)penInfo.font->GetKerning ( symbol, prevSymbol );

		GXFloat x = startX + penX;
		GXFloat y = startY + penY + info.offsetY;

		prevSymbol = symbol;
		GXSetMat4Scale ( mv, (GXFloat)info.width, (GXFloat)info.height, 1.0f );
		GXSetMat4TranslateTo ( mv, x, y, RENDER_Z );
		GXMulMat4Mat4 ( mvp, mv, projectionMatrix );

		glUniformMatrix4fv ( glyphModelViewProjectionMatrixLocation, 1, GL_FALSE, mvp.arr );
		glUniform4fv ( glyphColorLocation, 1, penInfo.color.arr );

		info.atlas->Bind ( TEXTURE_SLOT );
		glyphMesh.Render ();
		info.atlas->Unbind ();

		penX += (GXFloat)info.advance;
	}

	if ( bufferNumSymbols )
		free ( text );

	if ( penInfo.overlayType == eGXImageOverlayType::AlphaTransparencyPreserveAlpha )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glBindSampler ( TEXTURE_SLOT, 0 );

	glUseProgram ( 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
	glViewport ( oldVP[ 0 ], oldVP[ 1 ], oldVP[ 2 ], oldVP[ 3 ] );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );

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
	const GXMat4& view_proj_mat = GXCamera::GetActiveCamera ()->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	GLboolean isBlend;
	glGetBooleanv ( GL_BLEND, &isBlend );

	glDisable ( GL_CULL_FACE );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glUseProgram ( imageShaderProgram.GetProgram () );

	GXVec4 color ( 1.0f, 1.0f, 1.0f, 1.0f );

	glUniform4fv ( imageColorLocation, 1, color.arr );
	glUniformMatrix4fv ( imageModelViewProjectionMatrixLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

	glBindSampler ( TEXTURE_SLOT, sampler );
	texture.Bind ( TEXTURE_SLOT );

	surfaceMesh.Render ();

	texture.Unbind ();
	glBindSampler ( TEXTURE_SLOT, 0 );

	glUseProgram ( 0 );

	if ( !isBlend )
		glDisable ( GL_BLEND );
}

GXVoid GXHudSurface::InitGraphicResources ()
{
	GXUInt size = ( width * height ) * BYTES_PER_PIXEL;
	GXUByte* data = (GXUByte*)malloc ( size );
	memset ( data, 0, size );

	texture.InitResources ( width, height, GL_RGBA8, GX_FALSE );
	texture.FillWholePixelData ( data );
	free ( data );

	SetScale ( width * 0.5f, height * 0.5f, 1.0f );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetTextureObject (), 0 );
	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"GXHudSurface::GXHudSurfaceExt::Error - Что-то с fbo. Ошибка 0x%04x\n", status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	screenQuad = GXMeshGeometry::LoadFromStm ( L"3D Models/System/ScreenQuad1x1.stm" );
	surfaceMesh = GXMeshGeometry::LoadFromStm ( L"3D Models/System/ScreenQuad.stm" );

	static const GLchar* imageSamplerNames[ 1 ] = { "imageSampler" };
	static const GLuint imageSamplerLocations[ 1 ] = { 0 };

	GXShaderProgramInfo si;
	si.vs = L"Shaders/System/VertexAndUV_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/System/TextureWithColorAndAlphaTest_fs.txt";
	si.numSamplers = 1;
	si.samplerNames = imageSamplerNames;
	si.samplerLocations = imageSamplerLocations;

	imageShaderProgram = GXShaderProgram::GetShaderProgram ( si );

	imageModelViewProjectionMatrixLocation = imageShaderProgram.GetUniform ( "mod_view_proj_mat" );
	imageColorLocation = imageShaderProgram.GetUniform ( "color" );

	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 );
	glyphMesh.SetTotalVertices ( 6 );
	glyphMesh.SetBufferStream ( eGXMeshStreamIndex::Vertex, 3, GL_FLOAT, stride, (const GLvoid*)0 );
	glyphMesh.SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, (const GLvoid*)sizeof ( GXVec3 ) );
	glyphMesh.SetTopology ( GL_TRIANGLES );

	static const GLchar* textSamplerNames[ 1 ] = { "atlasSampler" };
	static const GLuint textSamplerLocations[ 1 ] = { 0 };

	si.vs = L"Shaders/System/VertexAndUV_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/System/Glyph_fs.txt";
	si.numSamplers = 1;
	si.samplerNames = textSamplerNames;
	si.samplerLocations = textSamplerLocations;

	glyphShaderProgram = GXShaderProgram::GetShaderProgram ( si );

	glyphModelViewProjectionMatrixLocation = glyphShaderProgram.GetUniform ( "mod_view_proj_mat" );
	glyphColorLocation = glyphShaderProgram.GetUniform ( "color" );

	lineMesh.SetTotalVertices ( 2 );
	lineMesh.SetBufferStream ( eGXMeshStreamIndex::Vertex, 3, GL_FLOAT, sizeof ( GXVec3 ), (const GLvoid*)0 );
	lineMesh.SetTopology ( GL_LINES );

	si.vs = L"Shaders/System/Line_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/System/Line_fs.txt";
	si.numSamplers = 0;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;

	lineShaderProgram = GXShaderProgram::GetShaderProgram ( si );

	lineModelViewProjectionMatrixLocation = lineShaderProgram.GetUniform ( "mod_view_proj_mat" );
	lineColorLocation = lineShaderProgram.GetUniform ( "color" );

	GXGLSamplerInfo samplerInfo;
	samplerInfo.anisotropy = 1.0f;
	samplerInfo.resampling = eGXSamplerResampling::None;
	samplerInfo.wrap = GL_CLAMP_TO_EDGE;
	sampler = GXCreateSampler ( samplerInfo );
}

GXVoid GXHudSurface::UpdateBounds ()
{
	//NOTHING
}

GXVoid GXHudSurface::UpdateGlyphGeometry ( const GXVec2 &min, const GXVec2 &max )
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

	glyphMesh.FillVertexBuffer ( buffer, 30 * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}

GXVoid GXHudSurface::UpdateLineGeometry ( const GXVec2 &start, const GXVec2 &end )
{
	static GXFloat buffer[ 6 ];
	buffer[ 0 ] = start.x;	buffer[ 1 ] = start.y;	buffer[ 2 ] = RENDER_Z;
	buffer[ 3 ] = end.x;	buffer[ 4 ] = end.y;	buffer[ 5 ] = RENDER_Z;

	lineMesh.FillVertexBuffer ( buffer, 6 * sizeof ( GXFloat ), GL_DYNAMIC_DRAW );
}
