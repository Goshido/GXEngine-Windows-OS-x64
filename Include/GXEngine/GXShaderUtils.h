//version 1.6

#ifndef GX_SHADER_UTILS
#define GX_SHADER_UTILS


#include "GXOpenGL.h"


//#define DEBUG_ERROR_AND_SHUTDOWN
//#define DEBUG_WARNING_AND_PAUSE
//#define DEBUG_PAUSE_AFTER_SHADERS
//#define DEBUG_OPENGL_ERROR_TO_CONSOLE


GLuint GXCALL GXCreateShaderProgram ( const GXWChar* vFileName, const GXWChar* gFileName, const GXWChar* fFileName );
GLuint GXCALL GXCreateTransformFeedbackShaderProgram ( const GXWChar* vFileName, const GXWChar* gFileName, const GXWChar* fFileName, GXUByte numVaryings, const GLchar** varyings, GLenum bufferMode );
GLint GXCALL GXGetUniformLocation ( GLint shaderProgram, const GXChar* uniform );


#endif //GX_SHADER_UTILS
