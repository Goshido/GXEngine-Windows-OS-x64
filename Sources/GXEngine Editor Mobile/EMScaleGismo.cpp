#include <GXEngine_Editor_Mobile/EMScaleGismo.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXCamera.h>


EMScaleGismo::EMScaleGismo ()
{
	isDeleted = GX_FALSE;
	isVisible = GX_TRUE;

	mod_view_proj_matLocation = -1;
}

GXVoid EMScaleGismo::Delete ()
{
	isDeleted = GX_TRUE;
}

GXVoid EMScaleGismo::Hide ()
{
	isVisible = GX_FALSE;
}

GXVoid EMScaleGismo::Show ()
{
	isVisible = GX_TRUE;
}

GXVoid EMScaleGismo::Draw ()
{
	if ( vaoInfo.vao == 0 )
	{
		Load3DModel ();
		InitUniforms ();
	}

	if ( isDeleted )
	{
		delete this;
		return;
	}

	if ( !isVisible ) return;

	glUseProgram ( programs[ 0 ].program );

	const GXMat4& view_proj_mat = GXCamera::GetActiveCamera ()->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, texture.texObj );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	glBindVertexArray ( 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glUseProgram ( 0 );
}

EMScaleGismo::~EMScaleGismo ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveTexture ( texture );
}

GXVoid EMScaleGismo::Load3DModel ()
{
	GXGetVAOFromNativeStaticMesh ( vaoInfo, L"3D Models/Editor Mobile/Scale Gismo.stm" );
	UpdateBounds ();

	GXLoadTexture ( L"Textures/Editor Mobile/Gismo Texture.tex", texture );
	glBindTexture ( GL_TEXTURE_2D, texture.texObj );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf ( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	AllocateShaderPrograms ( 1 );
	GXGetShaderProgram ( programs[ 0 ], L"Shaders/System/VertexAndUV_vs.txt", 0, L"Shaders/Editor Mobile/OneSampler_fs.txt" );
}

GXVoid EMScaleGismo::InitUniforms ()
{
	if ( !programs[ 0 ].isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "imageSampler" };

		GXTuneShaderSamplers ( programs[ 0 ], samplerIndexes, samplerNames, 1 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( programs[ 0 ].program, "mod_view_proj_mat" );
}
