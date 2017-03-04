//version 1.17

#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXFontStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXStrings.h>
#include <cstdio>


GXHudSurface::GXHudSurface ( GXUShort width, GXUShort height, GXBool enableSmooth )
{
	surfaceWidth = width;
	surfaceHeight = height;

	GXFloat aspect = width / (GXFloat)height;
	GXSetMat4Ortho ( imageProjMat, (GXFloat)surfaceWidth, (GXFloat)surfaceHeight, 0.0f, 20.0f );

	invAspect = 1.0f / aspect;

	Load3DModel ();
	InitUniforms ();

	GXUInt size = ( surfaceWidth * surfaceHeight ) << 2;

	GXUByte* data = (GXUByte*)malloc ( size );
	memset ( data, 0, size );

	GXGLTextureStruct ts;

	ts.data = data;
	ts.format = GL_RGBA;
	ts.internalFormat = GL_RGBA8;
	ts.type = GL_UNSIGNED_BYTE;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.width = surfaceWidth;
	ts.height = surfaceHeight;
	ts.resampling = enableSmooth ? GX_TEXTURE_RESAMPLING_LINEAR : GX_TEXTURE_RESAMPLING_NONE;

	texObj = GXCreateTexture ( ts );

	free ( data );

	GXMesh::SetScale ( surfaceWidth * 0.5f, surfaceHeight * 0.5f, 1.0f );

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0 );
	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogA ( "GXHudSurfaceExt::GXHudSurfaceExt::Error - Что-то с fbo. Ошибка 0x%04x\n", status );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
}

GXHudSurface::~GXHudSurface ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveVAO ( screenQuad );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &texObj );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	glDeleteBuffers ( 1, &uvVBO );
	glDeleteBuffers ( 1, &lineVBO );

	glBindVertexArray ( 0 );
	glDeleteVertexArrays ( 1, &lineVAO );

	glDeleteSamplers ( 1, &imageSampler );
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
	GXMat4 imageModMat;
	GXMat4 imageModProjMat;

	GXFloat halfWidth = imageInfo.insertWidth * 0.5f;
	GXFloat halfHeight = imageInfo.insertHeight * 0.5f;

	GXSetMat4Scale ( imageModMat, halfWidth, halfHeight, 1.0f );
	imageModMat.m41 = imageInfo.insertX + halfWidth - surfaceWidth * 0.5f;
	imageModMat.m42 = imageInfo.insertY + halfHeight - surfaceHeight * 0.5f;
	imageModMat.m43 = 1.0f;

	GXMulMat4Mat4 ( imageModProjMat, imageModMat, imageProjMat );

	GLint oldVP[ 4 ];
	glGetIntegerv ( GL_VIEWPORT, oldVP );

	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );

	if ( imageInfo.overlayType == GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

	GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, surfaceWidth, surfaceHeight );

	glUseProgram ( programs[ 0 ].program );

	glActiveTexture ( GL_TEXTURE0 );
	glBindSampler ( 0, imageSampler );
	glBindTexture ( GL_TEXTURE_2D, imageInfo.texture.texObj );

	glUniformMatrix4fv ( img_mod_view_proj_matLocation, 1, GL_FALSE, imageModProjMat.arr );
	glUniform4fv ( img_colorLocation, 1, imageInfo.color.arr );

	GLboolean isBlend;
	glGetBooleanv ( GL_BLEND, &isBlend );

	switch ( imageInfo.overlayType )
	{
		case GX_ALPHA_TRANSPARENCY:
		case GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			DrawImageQuad ();
			
			if ( !isBlend )
				glDisable ( GL_BLEND );
		}
		break;

		case GX_ALPHA_ADD:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_ONE, GL_ONE );

			DrawImageQuad ();

			if ( !isBlend )
				glDisable ( GL_BLEND );
		}
		break;

		case GX_ALPHA_MULTIPLY:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

			DrawImageQuad ();

			if ( !isBlend )
				glDisable ( GL_BLEND );
		}
		break;

		case GX_SIMPLE_REPLACE:
		default:
		{
			glDisable ( GL_BLEND );

			DrawImageQuad ();

			if ( isBlend )
				glEnable ( GL_BLEND );
		}
		break;
	}

	if ( imageInfo.overlayType == GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glBindTexture ( GL_TEXTURE_2D, 0 );
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
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );

	if ( lineInfo.overlayType == GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

	glViewport ( 0, 0, surfaceWidth, surfaceHeight );
	glUseProgram ( programs[ 2 ].program );

	UpdateLine ( lineInfo.startPoint, lineInfo.endPoint );

	GXMat4 imageModMat;
	GXSetMat4Translation ( imageModMat, -0.5f * (GXFloat)surfaceWidth, -0.5f * (GXFloat)surfaceHeight, 0.0f );

	GXMat4 imageModProjMat;
	GXMulMat4Mat4 ( imageModProjMat, imageModMat, imageProjMat );

	glUniform4fv ( line_colorLocation, 1, lineInfo.color.arr );
	glUniformMatrix4fv ( line_mod_view_proj_matLocation, 1, GX_FALSE, imageModProjMat.arr );

	switch ( lineInfo.overlayType )
	{
		case GX_ALPHA_TRANSPARENCY:
		case GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			DrawLine ();
		}
		break;

		case GX_ALPHA_ADD:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_ONE, GL_ONE );

			DrawLine ();
		}
		break;

		case GX_ALPHA_MULTIPLY:
		{
			glEnable ( GL_BLEND );
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

			DrawLine ();
		}
		break;

		case GX_SIMPLE_REPLACE:
		default:
		{
			glDisable ( GL_BLEND );
			DrawLine ();
		}
		break;
	}

	if ( lineInfo.overlayType == GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA )
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

	glViewport ( 0, 0, surfaceWidth, surfaceHeight );
	glUseProgram ( programs[ 1 ].program );

	glActiveTexture ( GL_TEXTURE0 );
	glBindSampler ( 0, imageSampler );

	if ( penInfo.overlayType == GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

	GXMat4 mvp;
	GXMat4 mv;

	GXFloat startX = -( surfaceWidth * 0.5f );
	GXFloat startY = -( surfaceHeight * 0.5f );

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
				penX += 4.0f * penInfo.font->GetSpaceAdvance ();
				continue;
			break;

			case '\n':
			case '\r':
				continue;
			break;
		}

		penInfo.font->GetGlyph ( symbol, info );

		UpdateUV ( info.min, info.max );

		if ( prevSymbol != 0 )
			penX += (GXFloat)penInfo.font->GetKerning ( symbol, prevSymbol );

		GXFloat x = startX + penX;
		GXFloat y = startY + penY + info.offsetY;

		prevSymbol = symbol;
		GXSetMat4Scale ( mv, (GXFloat)info.width, (GXFloat)info.height, 1.0f );
		GXSetMat4TranslateTo ( mv, x, y, 5.0f );
		GXMulMat4Mat4 ( mvp, mv, imageProjMat );

		glUniformMatrix4fv ( txt_mod_view_proj_matLocation, 1, GL_FALSE, mvp.arr );
		glUniform4fv ( txt_colorLocation, 1, penInfo.color.arr );
		glBindTexture ( GL_TEXTURE_2D, info.atlas );

		switch ( penInfo.overlayType )
		{
			case GX_ALPHA_TRANSPARENCY:
			case GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA:
			{
				glEnable ( GL_BLEND );
				glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

				DrawSymbolQuad ();
			}
			break;

			case GX_ALPHA_ADD:
			{
				glEnable ( GL_BLEND );
				glBlendFunc ( GL_ONE, GL_ONE );

				DrawSymbolQuad ();
			}
			break;

			case GX_ALPHA_MULTIPLY:
			{
				glEnable ( GL_BLEND );
				glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

				DrawSymbolQuad ();
			}
			break;

			case GX_SIMPLE_REPLACE:
			default:
			{
				glDisable ( GL_BLEND );

				DrawSymbolQuad ();
			}
			break;
		}

		penX += (GXFloat)info.advance;
	}

	if ( bufferNumSymbols )
		free ( text );

	if ( penInfo.overlayType == GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA )
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 0, 0 );

	glUseProgram ( 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
	glViewport ( oldVP[ 0 ], oldVP[ 1 ], oldVP[ 2 ], oldVP[ 3 ] );

	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );

	return penX;
}

GLuint GXHudSurface::GetTexture ()
{
	return texObj;
}

GXUShort GXHudSurface::GetWidth ()
{
	return (GXUShort)surfaceWidth;
}

GXUShort GXHudSurface::GetHeight ()
{
	return (GXUShort)surfaceHeight;
}

GXVoid GXHudSurface::Draw ()
{
	const GXMat4& view_proj_mat = GXCamera::GetActiveCamera ()->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	glDisable ( GL_CULL_FACE );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glUseProgram ( programs[ 0 ].program );

	GXVec4 color ( 1.0f, 1.0f, 1.0f, 1.0f );

	glUniform4fv ( img_colorLocation, 1, color.arr );
	glUniformMatrix4fv ( img_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, texObj );

	DrawImageQuad ();

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glUseProgram ( 0 );
}

GXVoid GXHudSurface::Load3DModel ()
{
	GXGetVAOFromNativeStaticMesh ( vaoInfo, L"3D Models/System/ScreenQuad.stm" );
	vaoInfo.bounds.min.z = -10.0f;
	vaoInfo.bounds.max.z = 10.0f;
	UpdateBounds ();

	GXGetVAOFromNativeStaticMesh ( screenQuad, L"3D Models/System/ScreenQuad1x1.stm" );
	glBindVertexArray ( screenQuad.vao );
	//{
		glGenBuffers ( 1, &uvVBO );
		glBindBuffer ( GL_ARRAY_BUFFER, uvVBO );
		glVertexAttribPointer ( GX_UV_COORD_INDEX, 2, GL_FLOAT, GL_FALSE, sizeof ( GXVec2 ), (const GLvoid*)0 );
		glEnableVertexAttribArray ( GX_UV_COORD_INDEX );
	//}
	glBindVertexArray ( 0 );

	glGenVertexArrays ( 1, &lineVAO );
	glBindVertexArray ( lineVAO );
	//{
		glGenBuffers ( 1, &lineVBO );
		glBindBuffer ( GL_ARRAY_BUFFER, lineVBO );

		glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec2 ), (const GLvoid*)0 );
		glEnableVertexAttribArray ( GX_VERTEX_INDEX );
	//}
	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	AllocateShaderPrograms ( 3 );
	GXGetShaderProgram ( programs[ 0 ], L"Shaders/System/VertexAndUV_vs.txt", 0, L"Shaders/System/TextureWithColorAndAlphaTest_fs.txt" );
	GXGetShaderProgram ( programs[ 1 ], L"Shaders/System/VertexAndUV_vs.txt", 0, L"Shaders/System/Glyph_fs.txt" );
	GXGetShaderProgram ( programs[ 2 ], L"Shaders/System/Line_vs.txt", 0, L"Shaders/System/Line_fs.txt" );

	GXGLSamplerStruct ss;
	ss.anisotropy = 1.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_NONE;
	ss.wrap = GL_CLAMP_TO_EDGE;
	imageSampler = GXCreateSampler ( ss );
}

GXVoid GXHudSurface::InitUniforms ()
{
	if ( !programs[ 0 ].isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "imageSampler" };

		GXTuneShaderSamplers ( programs[ 0 ], samplerIndexes, samplerNames, 1 );
	}

	img_mod_view_proj_matLocation = GXGetUniformLocation ( programs[ 0 ].program, "mod_view_proj_mat" );
	img_colorLocation = GXGetUniformLocation ( programs[ 0 ].program, "color" );

	if ( !programs[ 1 ].isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "atlasSampler" };

		GXTuneShaderSamplers ( programs[ 1 ], samplerIndexes, samplerNames, 1 );
	}

	txt_mod_view_proj_matLocation = GXGetUniformLocation ( programs[ 1 ].program, "mod_view_proj_mat" );
	txt_colorLocation = GXGetUniformLocation ( programs[ 1 ].program, "color" );

	line_colorLocation = GXGetUniformLocation ( programs[ 2 ].program, "color" );
	line_mod_view_proj_matLocation = GXGetUniformLocation ( programs[ 2 ].program, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}

GXVoid GXHudSurface::DrawImageQuad ()
{
	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	glBindVertexArray ( 0 );
}

GXVoid GXHudSurface::DrawSymbolQuad ()
{
	glBindVertexArray ( screenQuad.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuad.numVertices );
	glBindVertexArray ( 0 );
}

GXVoid GXHudSurface::DrawLine ()
{
	glBindVertexArray ( lineVAO );
	glDrawArrays ( GL_LINES, 0, 2 );
	glBindVertexArray ( 0 );
}

GXVoid GXHudSurface::UpdateUV ( const GXVec2 &min, const GXVec2 &max )
{
	GXFloat uv[ 12 ];
	uv[ 0 ] = min.x;		uv[ 1 ] = max.y;
	uv[ 2 ] = max.x;		uv[ 3 ] = min.y;
	uv[ 4 ] = min.x;		uv[ 5 ] = min.y;


	uv[ 6 ] = min.x;		uv[ 7 ] = max.y;
	uv[ 8 ] = max.x;		uv[ 9 ] = max.y;
	uv[ 10 ] = max.x;		uv[ 11 ] = min.y;
	
	glBindVertexArray ( screenQuad.vao );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, uvVBO );
			glBufferData ( GL_ARRAY_BUFFER, 12 * sizeof ( GXFloat ), uv, GL_DYNAMIC_DRAW );

		glVertexAttribPointer ( GX_UV_COORD_INDEX, 2, GL_FLOAT, GL_FALSE, sizeof ( GXVec2 ), (const GLvoid*)0 );
		glEnableVertexAttribArray ( GX_UV_COORD_INDEX );
	//}
	glBindVertexArray ( 0 );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}

GXVoid GXHudSurface::UpdateLine ( const GXVec3 &start, const GXVec3 &end )
{
	GXFloat data[ 6 ];
	data[ 0 ] = start.x;	data[ 1 ] = start.y;	data[ 2 ] = start.z;
	data[ 3 ] = end.x;		data[ 4 ] = end.y;		data[ 5 ] = end.z;
	
	glBindVertexArray ( lineVAO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, lineVBO );
			glBufferData ( GL_ARRAY_BUFFER, 6 * sizeof ( GXFloat ), data, GL_DYNAMIC_DRAW );

		glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec3 ), (const GLvoid*)0 );
		glEnableVertexAttribArray ( GX_VERTEX_INDEX );
	//}
	glBindVertexArray ( 0 );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}

//------------------------------------------------------------------------------------

GXTextBox::GXTextBox ( GXFont* font, const GXVec4 &fontColor, eGXTextAlignment alignment, GXUShort width, GXUShort height ):
GXHudSurface ( width, height, GX_FALSE ), pageInfo ( sizeof ( GXTextPageInfo ) )
{
	this->font = font;
	this->fontColor = fontColor;
	this->alignment = alignment;
	fontHeight = (GXInt)this->font->GetSize ();

	currentPage = 0;
	pageNum = 0;

	GXLogA ( "GXTextBox::GXTextBox::Info - Just worked\n" );
}

GXTextBox::~GXTextBox ()
{
	for ( GXByte i = 0; i < pageNum; i++ )
	{
		GXTextPageInfo* info = (GXTextPageInfo*)pageInfo.GetValue ( i );
		GXSafeDelete ( info->text );
	}

	GXLogA ( "GXTextBox::~GXTextBox::Info - Just worked\n" );
}

GXVoid GXTextBox::AddFullText ( const GXWChar* text )
{
	for ( GXByte i = 0; i < pageNum; i++ )
	{
		GXTextPageInfo* info = (GXTextPageInfo*)pageInfo.GetValue ( i );
		GXSafeDelete ( info->text );
	}

	pageInfo.Resize ( 0 );

	pageNum = 0;
	currentPage = 0;

	GXWChar* str;
	GXWcsclone ( &str, text );

	GXInt lineNum = 1;

	GXInt size = 0;
	GXInt offset = 0;

	GXInt pageMaxSize = 8192;
	GXWChar* pageText = (GXWChar*)malloc ( pageMaxSize * sizeof ( GXWChar ) );
	memset ( pageText, 0, pageMaxSize );

	GXBool recorded = GX_FALSE;
	GXUShort maxLines = surfaceHeight / fontHeight;
	GXInt strSize = GXWcslen ( str ) + 1;

	while ( strSize - size != 0 )
	{
		if ( lineNum > maxLines )
		{
			lineNum--;

			GXTextPageInfo info;
			info.text = (GXWChar*)malloc ( ( offset + 1 ) * sizeof ( GXWChar ) );
			memcpy ( info.text, pageText, offset * sizeof ( GXWChar ) );
			info.text[ offset ] = 0;
			info.numLines = (GXUInt)lineNum;
			pageInfo.SetValue ( pageNum, &info );

			pageNum++;
			offset = 0;
			lineNum = 1;

			recorded = GX_FALSE;
		}

		strSize = GXWcslen ( str ) + 1;
		GXWChar* resStr = GetTextFittingLine ( str, size );

		memmove ( str, str + size, ( GXWcslen ( str ) + 1 - size ) * sizeof ( GXWChar ) );
		memcpy ( pageText + offset, resStr, size );

		offset += size;

		GXSafeDelete ( resStr );

		lineNum++;
		recorded = GX_TRUE;
	}

	if ( recorded )
	{
		lineNum--;

		GXTextPageInfo info;
		info.text = (GXWChar*)malloc ( ( offset + 1 ) * sizeof ( GXWChar ) );
		memcpy ( info.text, pageText, offset * sizeof ( GXWChar ) );
		info.text[ offset ] = 0;
		info.numLines = (GXUInt)lineNum;
		pageInfo.SetValue ( pageNum, &info );
	}

	free ( pageText );
	GXSafeFree ( str );

	Update ();
}

GXUInt GXTextBox::GetPageLines ()
{
	GXTextPageInfo* info = (GXTextPageInfo*)pageInfo.GetValue ( currentPage );
	return info->numLines;
}

GXByte GXTextBox::GetPageNum ()
{
	return pageNum + 1;
}

GXVoid GXTextBox::NextPage ()
{
	if ( currentPage < pageNum )
	{
		currentPage++;
		Update ();
	}
}

GXVoid GXTextBox::PrevPage ()
{
	if ( currentPage > 0 )
	{
		currentPage--;
		Update ();
	}
}

GXVoid GXTextBox::SetPage ( GXUByte page )
{
	if ( (GXByte)page > pageNum )
	{
		currentPage = pageNum + 1;
		Reset ();
	}
	else
	{
		currentPage = page;
		Update ();
	}
}

GXVoid GXTextBox::SetText ( const GXWChar* text )
{
	GXPenInfo pi;

	GXWChar* str;
	GXWcsclone ( &str, text );

	GXInt lineNum = 1;
	pi.font = font;
	memcpy ( &pi.color, &fontColor, sizeof ( GXVec4 ) );
	pi.color = fontColor;
	pi.insertX = 0;
	pi.overlayType = GX_ALPHA_TRANSPARENCY;

	GXInt size = 0;
	GXInt strSize = GXWcslen ( str ) + 1;
	while ( strSize - size != 0 )
	{
		strSize = GXWcslen ( str ) + 1;
		GXWChar* resStr = GetTextFittingLine ( str, size );
		memmove ( str, str + size, ( GXWcslen ( str ) + 1 - size ) * sizeof ( GXUTF8 ) );
		GXInt currentLinePos = surfaceHeight - fontHeight * lineNum;
		pi.insertY = currentLinePos + 0.5f;

		switch ( alignment )
		{
			case GX_TEXT_ALIGNMENT_LEFT:
				pi.insertX = 0;
			break;

			case GX_TEXT_ALIGNMENT_CENTER:
			{
				GXInt size = (GXInt)font->GetTextLength ( 0, resStr );
				pi.insertX = ( (GXInt)surfaceWidth - size ) * 0.5f;
			}
			break;

			case GX_TEXT_ALIGNMENT_RIGHT:
			{
				GXInt size = (GXInt)font->GetTextLength ( 0, resStr );
				pi.insertX = (GXFloat)( (GXInt)surfaceWidth - size );
			}
			break;
		}

		AddText ( pi, 0, resStr );

		if ( resStr )
			delete resStr;

		lineNum++;
	}
	if ( str )
		free ( str );
}

GXWChar* GXTextBox::GetTextFittingLine ( const GXWChar* text, GXInt &size )
{
	GXWChar* str;
	GXWcsclone ( &str, text );

	GXInt space = 0;
	GXUInt pixSize = font->GetTextLength ( 0, str );
	GXWChar* testStr = 0;

	GXInt symbols = GXWcslen ( str );
	GXUInt symOffset = 0;

	GXBool firstWord = GX_TRUE;

	if ( pixSize > surfaceWidth )
	{
		//find the max string fitting the line

		for ( GXInt i = 0; i < symbols; i++ )
		{
			space++;

			if ( str[ i ] == ' ' || str[ i ] == '\n' )
			{
				testStr = new GXWChar[ space + 1 ];
				memcpy ( testStr, str, ( space - 1 ) * sizeof ( GXWChar ) );
				testStr[ space - 1 ] = 0;

				GXUInt testLength = font->GetTextLength ( 0, testStr );

				if ( testLength <= surfaceWidth )
				{
					symOffset = space;
					firstWord = GX_FALSE;
				}
				if ( testStr )
					delete testStr;

				if ( testLength > surfaceWidth )
				{
					if ( firstWord )
					{
						symOffset = space;

						testStr = new GXWChar[ symOffset + 1 ];

						memcpy ( testStr, str, symOffset * sizeof ( GXWChar ) );
						testStr[ symOffset ] = 0;

						size = symOffset;

						return testStr;
					}

					break;
				}
			}

			if ( str[ i ] == '\n' )
			{
				testStr = new GXWChar[ space + 1 ];

				memcpy ( testStr, str, space * sizeof ( GXWChar ) );
				testStr[ space ] = 0;

				symOffset = space;

				GXUInt testLength = font->GetTextLength ( 0, testStr );
				if ( testLength <= surfaceWidth )
				{
					symOffset = space;
					size = symOffset;
					return testStr;
				}
			}
		}

		testStr = new GXWChar[ symOffset + 1 ];

		memcpy ( testStr, str, symOffset * sizeof ( GXWChar ) );
		testStr[ symOffset ] = 0;

		size = symOffset;
	}
	else
	{
		GXUInt strNewLineSize = (GXUInt)wcscspn ( str, L"\n" );
		if ( strNewLineSize != GXWcslen ( str ) )
		{
			size = strNewLineSize + 1;
			testStr = new GXWChar[ size + 1 ];
			memcpy ( testStr, str, size * sizeof ( GXWChar ) );
			testStr[ size ] = 0;
		}
		else
		{
			size = ( GXWcslen ( str ) + 1 ) * sizeof ( GXWChar );
			testStr = new GXWChar[ GXWcslen ( str ) + 1 ];
			memcpy ( testStr, str, size );
		}
	}

	GXSafeFree ( str );

	return testStr;
}

GXVoid GXTextBox::Update ()
{
	Reset ();

	GXTextPageInfo* info = (GXTextPageInfo*)pageInfo.GetValue ( currentPage );
	SetText ( info->text );
}
