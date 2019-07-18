#include <GXEngine_Editor_Mobile/EMLightProbe.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_LOCATION_X                                      0.0f
#define DEFAULT_LOCATION_Y                                      0.0f
#define DEFAULT_LOCATION_Z                                      0.0f

#define DEFAULT_BOUND_RANGE_X                                   7.0f
#define DEFAULT_BOUND_RANGE_Y                                   7.0f
#define DEFAULT_BOUND_RANGE_Z                                   7.0f

#define DEFAULT_DIFFUSE_IRRADIANCE_CONVOLUTION_ANGLE_STEP       0.025f
#define DEFAULT_DIFFUSE_IRRADIANCE_RESOLUTION                   256u

#define DEFAULT_BRDF_INTEGRATION_MAP_RESOLUTION                 1024
#define DEFAULT_BRDF_INTEGRATION_MAP_SAMPLES_PER_PIXEL          8196

#define CUBE_MESH                                               L"Meshes/System/Unit Cube.stm"
#define SCREEN_QUAD_MESH                                        L"Meshes/System/ScreenQuad.stm"

//----------------------------------------------------------------------------

GXTexture2D EMLightProbe::_brdfIntegrationMap;
EMLightProbe* EMLightProbe::_probes = nullptr;

EMLightProbe::EMLightProbe ():
    _previous ( nullptr ),
    _next ( _probes ),
    _locationWorld ( DEFAULT_LOCATION_X, DEFAULT_LOCATION_Y, DEFAULT_LOCATION_Z )
{
    if ( _probes )
        _probes->_previous = this;

    _probes = this;

    constexpr GXFloat halfRangeX = 0.5f * DEFAULT_BOUND_RANGE_X;
    constexpr GXFloat halfRangeY = 0.5f * DEFAULT_BOUND_RANGE_Y;
    constexpr GXFloat halfRangeZ = 0.5f * DEFAULT_BOUND_RANGE_Z;

    _boundsWorld.AddVertex ( _locationWorld.GetX () - halfRangeX, _locationWorld.GetY () - halfRangeY, _locationWorld.GetZ () - halfRangeZ );
    _boundsWorld.AddVertex ( _locationWorld.GetX () + halfRangeX, _locationWorld.GetY () + halfRangeY, _locationWorld.GetZ () + halfRangeZ );

    _cube.LoadMesh ( CUBE_MESH );
    _screenQuad.LoadMesh ( SCREEN_QUAD_MESH );

    if ( _brdfIntegrationMap.IsInitialized () ) return;

    _brdfIntegratorMaterial.SetSamplesPerPixel ( DEFAULT_BRDF_INTEGRATION_MAP_SAMPLES_PER_PIXEL );
    SetBRDFIntegrationMapResolution ( DEFAULT_BRDF_INTEGRATION_MAP_RESOLUTION );
}

EMLightProbe::~EMLightProbe ()
{
    if ( _next ) _next->_previous = _previous;

    if ( _previous )
        _previous->_next = _next;
    else
        _probes = _next;

    if ( _diffuseIrradiance.IsInitialized () )
        _diffuseIrradiance.FreeResources ();

    if ( _prefilteredEnvironmentMap.IsInitialized () )
        _prefilteredEnvironmentMap.FreeResources ();

    if ( !_probes && _brdfIntegrationMap.IsInitialized () )
    {
        _brdfIntegrationMap.FreeResources ();
    }
}

GXVoid EMLightProbe::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    _locationWorld.Init ( x, y, z );
    SetBoundLocal ( _boundsWorld.GetWidth (), _boundsWorld.GetHeight (), _boundsWorld.GetDepth () );
}

GXVoid EMLightProbe::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
    _environmentMap = &cubeMap;
    UpdatePrefilteredEnvironmentMap ();

    if ( !_diffuseIrradiance.IsInitialized () )
        _diffuseIrradiance.InitResources ( DEFAULT_DIFFUSE_IRRADIANCE_RESOLUTION, GL_RGB16F, GL_FALSE );

    UpdateDiffuseIrradiance ();
}

GXVoid EMLightProbe::SetBoundLocal ( GXFloat xRange, GXFloat yRange, GXFloat zRange )
{
    const GXFloat halfRangeX = 0.5f * xRange;
    const GXFloat halfRangeY = 0.5f * yRange;
    const GXFloat halfRangeZ = 0.5f * zRange;

    _boundsWorld.Empty ();
    _boundsWorld.AddVertex ( _locationWorld.GetX () - halfRangeX, _locationWorld.GetY () - halfRangeY, _locationWorld.GetZ () - halfRangeZ );
    _boundsWorld.AddVertex ( _locationWorld.GetX () + halfRangeX, _locationWorld.GetY () + halfRangeY, _locationWorld.GetZ () + halfRangeZ );
}

GXUInt EMLightProbe::SetDiffuseIrradianceConvolutionAngleStep ( GXFloat radians )
{
    GXUInt samples = _diffuseIrradianceGeneratorMaterial.SetAngleStep ( radians );
    UpdateDiffuseIrradiance ();

    return samples;
}

GXVoid EMLightProbe::SetDiffuseIrradianceResolution ( GXUShort resolution )
{
    if ( _diffuseIrradiance.IsInitialized () )
        _diffuseIrradiance.FreeResources ();

    _diffuseIrradiance.InitResources ( resolution, GL_RGB16F, GX_FALSE );
    UpdateDiffuseIrradiance ();
}

GXVoid EMLightProbe::SetBRDFIntegrationMapResolution ( GXUShort length )
{
    if ( _brdfIntegrationMap.IsInitialized () )
        _brdfIntegrationMap.FreeResources ();

    _brdfIntegrationMap.InitResources ( length, length, GL_RG16F, GL_TRUE );
    UpdateBRDFIntegrationMap ();
}

GXVoid EMLightProbe::SetBRDFIntegrationMapSamplesPerPixel ( GXUShort samples )
{
    _brdfIntegratorMaterial.SetSamplesPerPixel ( samples );
    UpdateBRDFIntegrationMap ();
}

GXTextureCubeMap& EMLightProbe::GetDiffuseIrradiance ()
{
    return _diffuseIrradiance;
}

GXTextureCubeMap& EMLightProbe::GetPrefilteredEnvironmentMap ()
{
    return _prefilteredEnvironmentMap;
}

GXTexture2D& EMLightProbe::GetBRDFIntegrationMap ()
{
    return _brdfIntegrationMap;
}

GXVoid EMLightProbe::UpdateDiffuseIrradiance ()
{
    GXOpenGLState state;
    state.Save ();

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _diffuseIrradiance.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );
    glDisable ( GL_BLEND );

    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00u );
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

    GLsizei length = static_cast<GLsizei> ( _diffuseIrradiance.GetFaceLength () );
    glViewport ( 0, 0, length, length );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMLightProbe::UpdateDiffuseIrradiance::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    _diffuseIrradianceGeneratorMaterial.SetEnvironmentMap ( *_environmentMap );
    _diffuseIrradianceGeneratorMaterial.SetAngleStep ( DEFAULT_DIFFUSE_IRRADIANCE_CONVOLUTION_ANGLE_STEP );
    _diffuseIrradianceGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
    _cube.Render ();
    _diffuseIrradianceGeneratorMaterial.Unbind ();

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
    glDeleteFramebuffers ( 1, &fbo );

    state.Restore ();
}

GXVoid EMLightProbe::UpdatePrefilteredEnvironmentMap ()
{
    GXOpenGLState state;
    state.Save ();

    GXUShort length = _environmentMap->GetFaceLength ();

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );
    glDisable ( GL_BLEND );

    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00u );
    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

    glViewport ( 0, 0, static_cast<GLsizei> ( length ), static_cast<GLsizei> ( length ) );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    if ( _prefilteredEnvironmentMap.IsInitialized () )
        _prefilteredEnvironmentMap.FreeResources ();

    _prefilteredEnvironmentMap.InitResources ( length, GL_RGB16F, GL_TRUE );

    _prefilteredEnvironmentMapGeneratorMaterial.SetEnvironmentMap ( *_environmentMap );

    const GXFloat roughnessStep = 1.0f / static_cast<GXFloat> ( _prefilteredEnvironmentMap.GetLevelOfDetailNumber () - 1 );
    GLint mipmapLevel = 0;
    GLsizei currentResolution = static_cast<GLsizei> ( length );

    for ( GXFloat rougness = 0.0f; rougness < 1.0f; rougness += roughnessStep )
    {
        glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _prefilteredEnvironmentMap.GetTextureObject (), mipmapLevel );

        glViewport ( 0, 0, currentResolution, currentResolution );

        GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

        if ( status != GL_FRAMEBUFFER_COMPLETE )
        {
            GXLogA ( "EMLightProbe::UpdatePrefilteredEnvironmentMap::Error - Что-то не так на проходе генерации %i mipmap уровня FBO (ошибка 0x%08x)\n", mipmapLevel, status );
            ++mipmapLevel;
            continue;
        }

        _prefilteredEnvironmentMapGeneratorMaterial.SetRoughness ( rougness );
        _prefilteredEnvironmentMapGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
        _cube.Render ();
        _prefilteredEnvironmentMapGeneratorMaterial.Unbind ();

        currentResolution /= 2;

        ++mipmapLevel;
    }

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
    glDeleteFramebuffers ( 1, &fbo );

    state.Restore ();
}

GXVoid EMLightProbe::UpdateBRDFIntegrationMap ()
{
    GXOpenGLState state;
    state.Save ();

    GLuint fbo;
    glGenFramebuffers ( 1, &fbo );
    glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _brdfIntegrationMap.GetTextureObject (), 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0u, 0 );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0u, 0 );

    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );
    glDisable ( GL_BLEND );

    glDepthMask ( GL_FALSE );
    glStencilMask ( 0x00u );
    glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );

    glViewport ( 0, 0, static_cast<GLsizei> ( _brdfIntegrationMap.GetWidth () ), static_cast<GLsizei> ( _brdfIntegrationMap.GetHeight () ) );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMLightProbe::UpdateBRDFIntegrationMap::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    _brdfIntegratorMaterial.Bind ( GXTransform::GetNullTransform () );
    _screenQuad.Render ();
    _diffuseIrradianceGeneratorMaterial.Unbind ();

    glBindFramebuffer ( GL_FRAMEBUFFER, 0u );
    glDeleteFramebuffers ( 1, &fbo );

    state.Restore ();

    _brdfIntegrationMap.UpdateMipmaps ();
}

EMLightProbe* EMLightProbe::GetProbes ()
{
    return _probes;
}
