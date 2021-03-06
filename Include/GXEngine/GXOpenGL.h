// version 1.13

#ifndef GX_OPENGL
#define GX_OPENGL


#include <GXCommon/GXTypes.h>

GX_DISABLE_COMMON_WARNINGS

#include <gl/GL.h>
#include <OpenGL/glext.h>
#include <OpenGL/wglext.h>

GX_RESTORE_WARNING_STATE


extern PFNWGLCREATECONTEXTATTRIBSARBPROC        wglCreateContextAttribsARB;
extern PFNWGLSWAPINTERVALEXTPROC                wglSwapIntervalEXT;
extern PFNGLGETSHADERIVPROC                     glGetShaderiv;
extern PFNGLGETPROGRAMIVPROC                    glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC                glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC               glGetProgramInfoLog;
extern PFNGLGENVERTEXARRAYSPROC                 glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC                 glBindVertexArray;
extern PFNGLGENBUFFERSPROC                      glGenBuffers;
extern PFNGLBINDBUFFERPROC                      glBindBuffer;
extern PFNGLBUFFERDATAPROC                      glBufferData;
extern PFNGLBUFFERSUBDATAPROC                   glBufferSubData;
extern PFNGLUNMAPBUFFERPROC                     glUnmapBuffer;
extern PFNGLCREATEPROGRAMPROC                   glCreateProgram;
extern PFNGLCREATESHADERPROC                    glCreateShader;
extern PFNGLSHADERSOURCEPROC                    glShaderSource;
extern PFNGLCOMPILESHADERPROC                   glCompileShader;
extern PFNGLATTACHSHADERPROC                    glAttachShader;
extern PFNGLLINKPROGRAMPROC                     glLinkProgram;
extern PFNGLUSEPROGRAMPROC                      glUseProgram;
extern PFNGLVALIDATEPROGRAMPROC                 glValidateProgram;
extern PFNGLGETATTRIBLOCATIONPROC               glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC             glVertexAttribPointer;
extern PFNGLVERTEXATTRIB4NUBPROC                glVertexAttrib4Nub;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC         glEnableVertexAttribArray;
extern PFNGLGETUNIFORMLOCATIONPROC              glGetUniformLocation;
extern PFNGLDELETESHADERPROC                    glDeleteShader;
extern PFNGLDELETEPROGRAMPROC                   glDeleteProgram;
extern PFNGLUNIFORM1IPROC                       glUniform1i;
extern PFNGLUNIFORM1FPROC                       glUniform1f;
extern PFNGLUNIFORM1FVPROC                      glUniform1fv;
extern PFNGLUNIFORM2FPROC                       glUniform2f;
extern PFNGLUNIFORM2FVPROC                      glUniform2fv;
extern PFNGLUNIFORM3FVPROC                      glUniform3fv;
extern PFNGLUNIFORM4FPROC                       glUniform4f;
extern PFNGLUNIFORM4FVPROC                      glUniform4fv;
extern PFNGLUNIFORMMATRIX3FVPROC                glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC                glUniformMatrix4fv;
extern PFNGLDELETEVERTEXARRAYSPROC              glDeleteVertexArrays;
extern PFNGLDELETEBUFFERSPROC                   glDeleteBuffers;
extern PFNGLGENFRAMEBUFFERSPROC                 glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC                 glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC            glFramebufferTexture2D;
extern PFNGLDELETEFRAMEBUFFERSPROC              glDeleteFramebuffers;
extern PFNGLACTIVETEXTUREPROC                   glActiveTexture;
extern PFNGLBINDRENDERBUFFERPROC                glBindRenderbuffer;
extern PFNGLGENRENDERBUFFERSPROC                glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC             glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC         glFramebufferRenderbuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC          glCheckFramebufferStatus;
extern PFNGLDELETERENDERBUFFERSPROC             glDeleteRenderbuffers;
extern PFNGLDRAWBUFFERSPROC                     glDrawBuffers;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC        glDisableVertexAttribArray;
extern PFNGLDRAWRANGEELEMENTSPROC               glDrawRangeElements;
extern PFNGLBLENDEQUATIONPROC                   glBlendEquation;
extern PFNGLBLENDFUNCSEPARATEPROC               glBlendFuncSeparate;
extern PFNGLGENERATEMIPMAPPROC                  glGenerateMipmap;
extern PFNGLSTENCILOPSEPARATEPROC               glStencilOpSeparate;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC       glTransformFeedbackVaryings;
extern PFNGLBINDBUFFERBASEPROC                  glBindBufferBase;
extern PFNGLBINDBUFFERRANGEPROC                 glBindBufferRange;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC          glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC            glEndTransformFeedback;
extern PFNGLTEXIMAGE3DPROC                      glTexImage3D;
extern PFNGLFRAMEBUFFERTEXTUREPROC              glFramebufferTexture;
extern PFNGLGENSAMPLERSPROC                     glGenSamplers;
extern PFNGLDELETESAMPLERSPROC                  glDeleteSamplers;
extern PFNGLBINDSAMPLERPROC                     glBindSampler;
extern PFNGLSAMPLERPARAMETERIPROC               glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERFPROC               glSamplerParameterf;
extern PFNGLBLITFRAMEBUFFERPROC                 glBlitFramebuffer;
extern PFNGLCLEARBUFFERFVPROC                   glClearBufferfv;
extern PFNGLGETPROGRAMBINARYPROC                glGetProgramBinary;
extern PFNGLPROGRAMBINARYPROC                   glProgramBinary;
extern PFNGLPROGRAMPARAMETERIPROC               glProgramParameteri;


GXVoid GXCALL GXOpenGLInit ();
GXVoid GXCALL GXCheckOpenGLError ();


class GXOpenGLState final
{
    private:
        GLuint          _fbo;
        GLint           _viewport[ 4u ];
        GLboolean       _colorMask[ 4u ];

        GLboolean       _depthMask;
        GLboolean       _depthTest;
        GLfloat         _depthClearValue;
        GLint           _depthFunction;

        GLboolean       _cullFace;
        GLint           _cullFaceMode;

        GLboolean       _blending;

        GLenum          _drawBuffers[ 16u ];

    public:
        GXOpenGLState ();
        ~GXOpenGLState ();

        GXVoid Save ();
        GXVoid Restore ();

    private:
        GXOpenGLState ( const GXOpenGLState &other ) = delete;
        GXOpenGLState& operator = ( const GXOpenGLState &other ) = delete;
};


#endif // GX_OPENGL
