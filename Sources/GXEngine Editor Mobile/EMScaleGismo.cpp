#include <GXEngine_Editor_Mobile/EMScaleGismo.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXCamera.h>


#define TEXTURE_SLOT	0


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

GXVoid EMScaleGismo::Render ()
{
	if ( shaderProgram.GetProgram () == 0 )
		InitGraphicResources ();

	if ( isDeleted )
	{
		delete this;
		return;
	}

	if ( !isVisible ) return;

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();

	glUseProgram ( shaderProgram.GetProgram () );

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, activeCamera->GetViewProjectionMatrix () );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

	texture.Bind ( TEXTURE_SLOT );
	meshGeometry.Render ();
	texture.Unbind ();

	glUseProgram ( 0 );
}

EMScaleGismo::~EMScaleGismo ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
	GXMeshGeometry::RemoveMeshGeometry ( meshGeometry );
	GXTexture::RemoveTexture ( texture );
}

GXVoid EMScaleGismo::InitGraphicResources ()
{
	meshGeometry = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Scale Gismo.stm" );
	UpdateBounds ();

	texture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Gismo Texture.tex", GX_FALSE );

	const GLchar* samplerNames[ 1 ] = { "imageSampler" };
	const GLuint samplerLocations[ 1 ] = { 0 };

	GXShaderProgramInfo si;
	si.vs = L"Shaders/System/VertexAndUV_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/Editor Mobile/OneSampler_fs.txt";
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );
}

GXVoid EMScaleGismo::UpdateBounds ()
{
	if ( shaderProgram.GetProgram () == 0 ) return;
}
