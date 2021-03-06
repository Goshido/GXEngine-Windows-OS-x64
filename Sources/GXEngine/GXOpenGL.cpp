// version 1.13

#include <GXEngine/GXOpenGL.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_OPENGL_FRAMEBUFFER 0u

#define OPENGL_GET_PROC(p,n)                                                                    \
{                                                                                               \
    n = reinterpret_cast <p> ( reinterpret_cast <GXVoid*> ( wglGetProcAddress ( #n ) ) );       \
    if ( !n )                                                                                   \
        GXLogA ( "#n not found\n" );                                                            \
}


PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB      = nullptr;
PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT              = nullptr;
PFNGLGETSHADERIVPROC                    glGetShaderiv                   = nullptr;
PFNGLGETPROGRAMIVPROC                   glGetProgramiv                  = nullptr;
PFNGLGETSHADERINFOLOGPROC               glGetShaderInfoLog              = nullptr;
PFNGLGETPROGRAMINFOLOGPROC              glGetProgramInfoLog             = nullptr;
PFNGLGENVERTEXARRAYSPROC                glGenVertexArrays               = nullptr;
PFNGLBINDVERTEXARRAYPROC                glBindVertexArray               = nullptr;
PFNGLGENBUFFERSPROC                     glGenBuffers                    = nullptr;
PFNGLBINDBUFFERPROC                     glBindBuffer                    = nullptr;
PFNGLBUFFERDATAPROC                     glBufferData                    = nullptr;
PFNGLBUFFERSUBDATAPROC                  glBufferSubData                 = nullptr;
PFNGLCREATEPROGRAMPROC                  glCreateProgram                 = nullptr;
PFNGLCREATESHADERPROC                   glCreateShader                  = nullptr;
PFNGLSHADERSOURCEPROC                   glShaderSource                  = nullptr;
PFNGLCOMPILESHADERPROC                  glCompileShader                 = nullptr;
PFNGLATTACHSHADERPROC                   glAttachShader                  = nullptr;
PFNGLLINKPROGRAMPROC                    glLinkProgram                   = nullptr;
PFNGLUSEPROGRAMPROC                     glUseProgram                    = nullptr;
PFNGLVALIDATEPROGRAMPROC                glValidateProgram               = nullptr;
PFNGLGETATTRIBLOCATIONPROC              glGetAttribLocation             = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC            glVertexAttribPointer           = nullptr;
PFNGLVERTEXATTRIB4NUBPROC               glVertexAttrib4Nub              = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC        glEnableVertexAttribArray       = nullptr;
PFNGLGETUNIFORMLOCATIONPROC             glGetUniformLocation            = nullptr;
PFNGLDELETESHADERPROC                   glDeleteShader                  = nullptr;
PFNGLDELETEPROGRAMPROC                  glDeleteProgram                 = nullptr;
PFNGLUNIFORM1IPROC                      glUniform1i                     = nullptr;
PFNGLUNIFORM1FPROC                      glUniform1f                     = nullptr;
PFNGLUNIFORM1FVPROC                     glUniform1fv                    = nullptr;
PFNGLUNIFORM2FPROC                      glUniform2f                     = nullptr;
PFNGLUNIFORM2FVPROC                     glUniform2fv                    = nullptr;
PFNGLUNIFORM3FVPROC                     glUniform3fv                    = nullptr;
PFNGLUNIFORM4FPROC                      glUniform4f                     = nullptr;
PFNGLUNIFORM4FVPROC                     glUniform4fv                    = nullptr;
PFNGLUNIFORMMATRIX3FVPROC               glUniformMatrix3fv              = nullptr;
PFNGLUNIFORMMATRIX4FVPROC               glUniformMatrix4fv              = nullptr;
PFNGLDELETEVERTEXARRAYSPROC             glDeleteVertexArrays            = nullptr;
PFNGLDELETEBUFFERSPROC                  glDeleteBuffers                 = nullptr;
PFNGLGENFRAMEBUFFERSPROC                glGenFramebuffers               = nullptr;
PFNGLBINDFRAMEBUFFERPROC                glBindFramebuffer               = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC           glFramebufferTexture2D          = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC             glDeleteFramebuffers            = nullptr;
PFNGLACTIVETEXTUREPROC                  glActiveTexture                 = nullptr;
PFNGLBINDRENDERBUFFERPROC               glBindRenderbuffer              = nullptr;
PFNGLGENRENDERBUFFERSPROC               glGenRenderbuffers              = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC            glRenderbufferStorage           = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC        glFramebufferRenderbuffer       = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC         glCheckFramebufferStatus        = nullptr;
PFNGLDELETERENDERBUFFERSPROC            glDeleteRenderbuffers           = nullptr;
PFNGLDRAWBUFFERSPROC                    glDrawBuffers                   = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC       glDisableVertexAttribArray      = nullptr;
PFNGLDRAWRANGEELEMENTSPROC              glDrawRangeElements             = nullptr;
PFNGLBLENDEQUATIONPROC                  glBlendEquation                 = nullptr;
PFNGLBLENDFUNCSEPARATEPROC              glBlendFuncSeparate             = nullptr;
PFNGLGENERATEMIPMAPPROC                 glGenerateMipmap                = nullptr;
PFNGLSTENCILOPSEPARATEPROC              glStencilOpSeparate             = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC      glTransformFeedbackVaryings     = nullptr;
PFNGLBINDBUFFERBASEPROC                 glBindBufferBase                = nullptr;
PFNGLBINDBUFFERRANGEPROC                glBindBufferRange               = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKPROC         glBeginTransformFeedback        = nullptr;
PFNGLENDTRANSFORMFEEDBACKPROC           glEndTransformFeedback          = nullptr;
PFNGLTEXIMAGE3DPROC                     glTexImage3D                    = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC             glFramebufferTexture            = nullptr;
PFNGLGENSAMPLERSPROC                    glGenSamplers                   = nullptr;
PFNGLDELETESAMPLERSPROC                 glDeleteSamplers                = nullptr;
PFNGLBINDSAMPLERPROC                    glBindSampler                   = nullptr;
PFNGLSAMPLERPARAMETERIPROC              glSamplerParameteri             = nullptr;
PFNGLSAMPLERPARAMETERFPROC              glSamplerParameterf             = nullptr;
PFNGLBLITFRAMEBUFFERPROC                glBlitFramebuffer               = nullptr;
PFNGLCLEARBUFFERFVPROC                  glClearBufferfv                 = nullptr;
PFNGLGETPROGRAMBINARYPROC               glGetProgramBinary              = nullptr;
PFNGLPROGRAMBINARYPROC                  glProgramBinary                 = nullptr;
PFNGLPROGRAMPARAMETERIPROC              glProgramParameteri             = nullptr;


GXVoid GXCALL GXOpenGLInit ()
{
    OPENGL_GET_PROC ( PFNWGLCREATECONTEXTATTRIBSARBPROC,    wglCreateContextAttribsARB      );
    OPENGL_GET_PROC ( PFNWGLSWAPINTERVALEXTPROC,            wglSwapIntervalEXT              );
    OPENGL_GET_PROC ( PFNGLGETSHADERIVPROC,                 glGetShaderiv                   );
    OPENGL_GET_PROC ( PFNGLGETPROGRAMIVPROC,                glGetProgramiv                  );
    OPENGL_GET_PROC ( PFNGLGETSHADERINFOLOGPROC,            glGetShaderInfoLog              );
    OPENGL_GET_PROC ( PFNGLGETPROGRAMINFOLOGPROC,           glGetProgramInfoLog             );
    OPENGL_GET_PROC ( PFNGLGENVERTEXARRAYSPROC,             glGenVertexArrays               );
    OPENGL_GET_PROC ( PFNGLBINDVERTEXARRAYPROC,             glBindVertexArray               );
    OPENGL_GET_PROC ( PFNGLGENBUFFERSPROC,                  glGenBuffers                    );
    OPENGL_GET_PROC ( PFNGLBINDBUFFERPROC,                  glBindBuffer                    );
    OPENGL_GET_PROC ( PFNGLBUFFERDATAPROC,                  glBufferData                    );
    OPENGL_GET_PROC ( PFNGLBUFFERSUBDATAPROC,               glBufferSubData                 );
    OPENGL_GET_PROC ( PFNGLCREATEPROGRAMPROC,               glCreateProgram                 );
    OPENGL_GET_PROC ( PFNGLCREATESHADERPROC,                glCreateShader                  );
    OPENGL_GET_PROC ( PFNGLSHADERSOURCEPROC,                glShaderSource                  );
    OPENGL_GET_PROC ( PFNGLCOMPILESHADERPROC,               glCompileShader                 );
    OPENGL_GET_PROC ( PFNGLATTACHSHADERPROC,                glAttachShader                  );
    OPENGL_GET_PROC ( PFNGLLINKPROGRAMPROC,                 glLinkProgram                   );
    OPENGL_GET_PROC ( PFNGLUSEPROGRAMPROC,                  glUseProgram                    );
    OPENGL_GET_PROC ( PFNGLVALIDATEPROGRAMPROC,             glValidateProgram               );
    OPENGL_GET_PROC ( PFNGLGETATTRIBLOCATIONPROC,           glGetAttribLocation             );
    OPENGL_GET_PROC ( PFNGLVERTEXATTRIBPOINTERPROC,         glVertexAttribPointer           );
    OPENGL_GET_PROC ( PFNGLVERTEXATTRIB4NUBPROC,            glVertexAttrib4Nub              );
    OPENGL_GET_PROC ( PFNGLENABLEVERTEXATTRIBARRAYPROC,     glEnableVertexAttribArray       );
    OPENGL_GET_PROC ( PFNGLGETUNIFORMLOCATIONPROC,          glGetUniformLocation            );
    OPENGL_GET_PROC ( PFNGLDELETESHADERPROC,                glDeleteShader                  );
    OPENGL_GET_PROC ( PFNGLDELETEPROGRAMPROC,               glDeleteProgram                 );
    OPENGL_GET_PROC ( PFNGLUNIFORM1IPROC,                   glUniform1i                     );
    OPENGL_GET_PROC ( PFNGLUNIFORM1FPROC,                   glUniform1f                     );
    OPENGL_GET_PROC ( PFNGLUNIFORM1FVPROC,                  glUniform1fv                    );
    OPENGL_GET_PROC ( PFNGLUNIFORM2FPROC,                   glUniform2f                     );
    OPENGL_GET_PROC ( PFNGLUNIFORM2FVPROC,                  glUniform2fv                    );
    OPENGL_GET_PROC ( PFNGLUNIFORM3FVPROC,                  glUniform3fv                    );
    OPENGL_GET_PROC ( PFNGLUNIFORM4FPROC,                   glUniform4f                     );
    OPENGL_GET_PROC ( PFNGLUNIFORM4FVPROC,                  glUniform4fv                    );
    OPENGL_GET_PROC ( PFNGLUNIFORMMATRIX3FVPROC,            glUniformMatrix3fv              );
    OPENGL_GET_PROC ( PFNGLUNIFORMMATRIX4FVPROC,            glUniformMatrix4fv              );
    OPENGL_GET_PROC ( PFNGLDELETEVERTEXARRAYSPROC,          glDeleteVertexArrays            );
    OPENGL_GET_PROC ( PFNGLDELETEBUFFERSPROC,               glDeleteBuffers                 );
    OPENGL_GET_PROC ( PFNGLGENFRAMEBUFFERSPROC,             glGenFramebuffers               );
    OPENGL_GET_PROC ( PFNGLBINDFRAMEBUFFERPROC,             glBindFramebuffer               );
    OPENGL_GET_PROC ( PFNGLFRAMEBUFFERTEXTURE2DPROC,        glFramebufferTexture2D          );
    OPENGL_GET_PROC ( PFNGLDELETEFRAMEBUFFERSPROC,          glDeleteFramebuffers            );
    OPENGL_GET_PROC ( PFNGLACTIVETEXTUREPROC,               glActiveTexture                 );
    OPENGL_GET_PROC ( PFNGLBINDRENDERBUFFERPROC,            glBindRenderbuffer              );
    OPENGL_GET_PROC ( PFNGLGENRENDERBUFFERSPROC,            glGenRenderbuffers              );
    OPENGL_GET_PROC ( PFNGLRENDERBUFFERSTORAGEPROC,         glRenderbufferStorage           );
    OPENGL_GET_PROC ( PFNGLFRAMEBUFFERRENDERBUFFERPROC,     glFramebufferRenderbuffer       );
    OPENGL_GET_PROC ( PFNGLCHECKFRAMEBUFFERSTATUSPROC,      glCheckFramebufferStatus        );
    OPENGL_GET_PROC ( PFNGLDELETERENDERBUFFERSPROC,         glDeleteRenderbuffers           );
    OPENGL_GET_PROC ( PFNGLDRAWBUFFERSPROC,                 glDrawBuffers                   );
    OPENGL_GET_PROC ( PFNGLDISABLEVERTEXATTRIBARRAYPROC,    glDisableVertexAttribArray      );
    OPENGL_GET_PROC ( PFNGLDRAWRANGEELEMENTSPROC,           glDrawRangeElements             );
    OPENGL_GET_PROC ( PFNGLBLENDEQUATIONPROC,               glBlendEquation                 );
    OPENGL_GET_PROC ( PFNGLBLENDFUNCSEPARATEPROC,           glBlendFuncSeparate             );
    OPENGL_GET_PROC ( PFNGLGENERATEMIPMAPPROC,              glGenerateMipmap                );
    OPENGL_GET_PROC ( PFNGLSTENCILOPSEPARATEPROC,           glStencilOpSeparate             );
    OPENGL_GET_PROC ( PFNGLTRANSFORMFEEDBACKVARYINGSPROC,   glTransformFeedbackVaryings     );
    OPENGL_GET_PROC ( PFNGLBINDBUFFERBASEPROC,              glBindBufferBase                );
    OPENGL_GET_PROC ( PFNGLBINDBUFFERRANGEPROC,             glBindBufferRange               );
    OPENGL_GET_PROC ( PFNGLBEGINTRANSFORMFEEDBACKPROC,      glBeginTransformFeedback        );
    OPENGL_GET_PROC ( PFNGLENDTRANSFORMFEEDBACKPROC,        glEndTransformFeedback          );
    OPENGL_GET_PROC ( PFNGLTEXIMAGE3DPROC,                  glTexImage3D                    );
    OPENGL_GET_PROC ( PFNGLFRAMEBUFFERTEXTUREPROC,          glFramebufferTexture            );
    OPENGL_GET_PROC ( PFNGLGENSAMPLERSPROC,                 glGenSamplers                   );
    OPENGL_GET_PROC ( PFNGLDELETESAMPLERSPROC,              glDeleteSamplers                );
    OPENGL_GET_PROC ( PFNGLBINDSAMPLERPROC,                 glBindSampler                   );
    OPENGL_GET_PROC ( PFNGLSAMPLERPARAMETERIPROC,           glSamplerParameteri             );
    OPENGL_GET_PROC ( PFNGLSAMPLERPARAMETERFPROC,           glSamplerParameterf             );
    OPENGL_GET_PROC ( PFNGLBLITFRAMEBUFFERPROC,             glBlitFramebuffer               );
    OPENGL_GET_PROC ( PFNGLCLEARBUFFERFVPROC,               glClearBufferfv                 );
    OPENGL_GET_PROC ( PFNGLGETPROGRAMBINARYPROC,            glGetProgramBinary              );
    OPENGL_GET_PROC ( PFNGLPROGRAMBINARYPROC,               glProgramBinary                 );
    OPENGL_GET_PROC ( PFNGLPROGRAMPARAMETERIPROC,           glProgramParameteri             );
}

GXVoid GXCALL GXCheckOpenGLError ()
{
    GLenum error = glGetError ();

    if ( error == GL_NO_ERROR ) return;

    GXLogA ( "GXCheckOpenGLError::Error 0x%08X\n", static_cast<GXUInt> ( error ) );
}

//-----------------------------------------------------------------------------------------

GXOpenGLState::GXOpenGLState ()
{
    // NOTHING
}

GXOpenGLState::~GXOpenGLState ()
{
    // NOTHING
}

GXVoid GXOpenGLState::Save ()
{
    glGetIntegerv ( GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*> ( &_fbo ) );
    glGetIntegerv ( GL_VIEWPORT, _viewport );
    glGetBooleanv ( GL_COLOR_WRITEMASK, _colorMask );

    glGetBooleanv ( GL_DEPTH_WRITEMASK, &_depthMask );
    glGetBooleanv ( GL_DEPTH_TEST, &_depthTest );
    glGetFloatv ( GL_DEPTH_CLEAR_VALUE, &_depthClearValue );
    glGetIntegerv ( GL_DEPTH_FUNC, &_depthFunction );

    glGetBooleanv ( GL_CULL_FACE, &_cullFace );
    glGetIntegerv ( GL_CULL_FACE_MODE, &_cullFaceMode );

    GLint maxDrawBuffers = 0;
    glGetIntegerv ( GL_MAX_DRAW_BUFFERS, &maxDrawBuffers );

    for ( GLint i = 0; i < maxDrawBuffers; ++i )
        glGetIntegerv ( static_cast<GLenum> ( GL_DRAW_BUFFER0 + i ), reinterpret_cast<GLint*> ( _drawBuffers + i ) );

    GXCheckOpenGLError ();
}

GXVoid GXOpenGLState::Restore ()
{
    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );

    glViewport ( _viewport[ 0 ], _viewport[ 1 ], _viewport[ 2 ], _viewport[ 3 ] );
    glColorMask ( _colorMask[ 0 ], _colorMask[ 1 ], _colorMask[ 2 ], _colorMask[ 3 ] );

    glDepthMask ( _depthMask );
    glClearDepth ( _depthClearValue );
    glDepthFunc ( static_cast<GLenum> ( _depthFunction ) );

    glCullFace ( static_cast<GLenum> ( _cullFaceMode ) );

    if ( _depthTest )
        glEnable ( GL_DEPTH_TEST );
    else
        glDisable ( GL_DEPTH_TEST );

    if ( _cullFace )
        glEnable ( GL_CULL_FACE );
    else
        glDisable ( GL_CULL_FACE );

    if ( _blending )
        glEnable ( GL_BLEND );
    else
        glDisable ( GL_BLEND );

    if ( _fbo == DEFAULT_OPENGL_FRAMEBUFFER )
    {
        glDrawBuffer ( _drawBuffers[ 0 ] );
    }
    else
    {
        GLint maxDrawBuffers = 0;
        glGetIntegerv ( GL_MAX_DRAW_BUFFERS, &maxDrawBuffers );
        glDrawBuffers ( maxDrawBuffers, _drawBuffers );
    }

    GXCheckOpenGLError ();
}
