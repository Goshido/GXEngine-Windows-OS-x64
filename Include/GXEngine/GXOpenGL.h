//version 1.6

#ifndef GX_OPENGL
#define GX_OPENGL


#include <GXCommon/GXTypes.h>
#include <gl/GL.h>
#include <OpenGL/glext.h>
#include <OpenGL/wglext.h>


extern PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB;
extern PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT;
extern PFNGLGETSHADERIVPROC					glGetShaderiv;
extern PFNGLGETPROGRAMIVPROC				glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
extern PFNGLGENVERTEXARRAYSPROC				glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC				glBindVertexArray;
extern PFNGLGENBUFFERSPROC					glGenBuffers;
extern PFNGLBINDBUFFERPROC					glBindBuffer;
extern PFNGLBUFFERDATAPROC					glBufferData;
extern PFNGLUNMAPBUFFERPROC					glUnmapBuffer;
extern PFNGLCREATEPROGRAMPROC				glCreateProgram;
extern PFNGLCREATESHADERPROC				glCreateShader;
extern PFNGLSHADERSOURCEPROC				glShaderSource;
extern PFNGLCOMPILESHADERPROC				glCompileShader;
extern PFNGLATTACHSHADERPROC				glAttachShader;
extern PFNGLLINKPROGRAMPROC					glLinkProgram;
extern PFNGLUSEPROGRAMPROC					glUseProgram;
extern PFNGLVALIDATEPROGRAMPROC				glValidateProgram;
extern PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer;
extern PFNGLVERTEXATTRIB4NUBPROC			glVertexAttrib4Nub;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC		glEnableVertexAttribArray;
extern PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
extern PFNGLDELETESHADERPROC				glDeleteShader;
extern PFNGLDELETEPROGRAMPROC				glDeleteProgram;
extern PFNGLUNIFORM1IPROC					glUniform1i;
extern PFNGLUNIFORM1FPROC					glUniform1f;
extern PFNGLUNIFORM1FVPROC					glUniform1fv;
extern PFNGLUNIFORM2FVPROC					glUniform2fv;
extern PFNGLUNIFORM3FVPROC					glUniform3fv;
extern PFNGLUNIFORM4FPROC					glUniform4f;
extern PFNGLUNIFORM4FVPROC					glUniform4fv;
extern PFNGLUNIFORMMATRIX3FVPROC			glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv;
extern PFNGLDELETEVERTEXARRAYSPROC			glDeleteVertexArrays;
extern PFNGLDELETEBUFFERSPROC				glDeleteBuffers;
extern PFNGLGENFRAMEBUFFERSPROC				glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC				glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC		glFramebufferTexture2D;
extern PFNGLDELETEFRAMEBUFFERSPROC			glDeleteFramebuffers;
extern PFNGLACTIVETEXTUREPROC				glActiveTexture;
extern PFNGLBINDRENDERBUFFERPROC			glBindRenderbuffer;
extern PFNGLGENRENDERBUFFERSPROC			glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC			glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC		glFramebufferRenderbuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC		glCheckFramebufferStatus;
extern PFNGLDELETERENDERBUFFERSPROC			glDeleteRenderbuffers;
extern PFNGLDRAWBUFFERSPROC					glDrawBuffers;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
extern PFNGLDRAWRANGEELEMENTSPROC			glDrawRangeElements;
extern PFNGLBLENDEQUATIONPROC				glBlendEquation;
extern PFNGLBLENDFUNCSEPARATEPROC			glBlendFuncSeparate;
extern PFNGLGENERATEMIPMAPPROC				glGenerateMipmap;
extern PFNGLSTENCILOPSEPARATEPROC			glStencilOpSeparate;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC	glTransformFeedbackVaryings;
extern PFNGLBINDBUFFERBASEPROC				glBindBufferBase;
extern PFNGLBINDBUFFERRANGEPROC				glBindBufferRange;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC		glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC		glEndTransformFeedback;
extern PFNGLTEXIMAGE3DPROC					glTexImage3D;
extern PFNGLFRAMEBUFFERTEXTUREPROC			glFramebufferTexture;
extern PFNGLGENSAMPLERSPROC					glGenSamplers;
extern PFNGLDELETESAMPLERSPROC				glDeleteSamplers;
extern PFNGLBINDSAMPLERPROC					glBindSampler;
extern PFNGLSAMPLERPARAMETERIPROC			glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERFPROC			glSamplerParameterf;


GXVoid GXCALL GXOpenGLInit ();
GXVoid GXCALL GXCheckOpenGLError ();


#endif //GX_OPENGL