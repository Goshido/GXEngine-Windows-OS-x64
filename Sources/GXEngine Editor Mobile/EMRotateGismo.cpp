#include <GXEngine_Editor_Mobile/EMRotateGismo.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXSamplerUtils.h>


EMRotateGismo::EMRotateGismo ()
{
	isDeleted = GX_FALSE;
	isVisible = GX_TRUE;

	mod_view_proj_matLocation = -1;
}

GXVoid EMRotateGismo::Delete ()
{
	isDeleted = GX_TRUE;
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
	if ( shaderProgram.GetProgram () == 0 )
		InitGraphicResources ();

	if ( isDeleted )
	{
		delete this;
		return;
	}

	if ( !isVisible ) return;

	GXCamera* camera = GXCamera::GetActiveCamera ();

	if ( !camera->IsObjectVisible ( meshGeometry.GetBoundsWorld () ) ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	const GXMat4& view_proj_mat = camera->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );

	texture.Bind ( 0 );
	meshGeometry.Render ();

	texture.Unbind ();
	glUseProgram ( 0 );
}

EMRotateGismo::~EMRotateGismo ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
	GXMeshGeometry::RemoveMeshGeometry ( meshGeometry );
	GXTexture::RemoveTexture ( texture );
	glDeleteSamplers ( 1, &sampler );
}

GXVoid EMRotateGismo::InitGraphicResources ()
{
	meshGeometry = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Rotate Gismo.stm" );
	UpdateBounds ();

	static const GLchar* samplerNames[ 1 ] = { "imageSampler" };
	static const GLuint samplerLocations[ 1 ] = { 0 };

	GXShaderProgramInfo si;
	si.vs = L"Shaders/System/VertexAndUV_vs.txt";
	si.gs = nullptr;
	si.fs = L"Shaders/Editor Mobile/OneSampler_fs.txt";
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	mod_view_proj_matLocation = shaderProgram.GetUniform ( "mod_view_proj_mat" );

	texture = GXTexture::LoadTexture ( L"Textures/Editor Mobile/Gismo Texture.tex", GX_FALSE );

	GXGLSamplerInfo samplerInfo;
	samplerInfo.anisotropy = 1.0f;
	samplerInfo.resampling = eGXSamplerResampling::None;
	samplerInfo.wrap = GL_CLAMP_TO_EDGE;

	sampler = GXCreateSampler ( samplerInfo );
}

GXVoid EMRotateGismo::UpdateBounds ()
{
	meshGeometry.UpdateBoundsWorld ( mod_mat );
}
