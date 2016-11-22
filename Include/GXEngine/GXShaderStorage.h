//version 1.9

#ifndef GX_SHADER_STORAGE
#define GX_SHADER_STORAGE


#include "GXOpenGL.h"


struct GXShaderProgram
{
	GLuint			program;
	GXBool			isSamplersTuned;

	GXShaderProgram ();
};

GXVoid GXCALL GXGetShaderProgram ( GXShaderProgram &program, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs );
GXVoid GXCALL GXGetTransformFeedbackShaderProgram ( GXShaderProgram &program, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, GXUByte numVaryings, const GLchar** varyings, GLenum bufferMode );
GXVoid GXCALL GXRemoveShaderProgram ( const GXShaderProgram &program );
GXVoid GXCALL GXTuneShaderSamplers ( GXShaderProgram &program, const GLuint* samplerIndexes, const GLchar** samplerNames, GXUByte numSamplers );

GXUInt GXCALL GXGetTotalShaderStorageObjects ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );


#endif //GX_SHADER_STORAGE
