// version 1.26

#ifndef GX_HUD_SURFACE
#define GX_HUD_SURFACE


#include "GXCameraOrthographic.h"
#include "GXFont.h"
#include "GXMeshGeometry.h"
#include "GXRenderable.h"
#include "GXTexture2D.h"
#include "GXTransform.h"
#include "GXUnlitColorMaskMaterial.h"
#include "GXUnlitColorMaterial.h"
#include "GXUnlitTexture2DMaterial.h"


enum class eGXImageOverlayType
{
    AlphaTransparency,
    AlphaTransparencyPreserveAlpha,
    AlphaAdd,
    AlphaMultiply,
    SimpleReplace
};

struct GXImageInfo final
{
    GXTexture2D*            _texture;

    GXColorRGB              _color;
    GXFloat                 _insertX;
    GXFloat                 _insertY;
    GXFloat                 _insertWidth;
    GXFloat                 _insertHeight;

    eGXImageOverlayType     _overlayType;
};

struct GXPenInfo final
{
    GXFont*                 _font;
    GXColorRGB              _color;
    GXFloat                 _insertX;
    GXFloat                 _insertY;

    eGXImageOverlayType     _overlayType;
};

struct GXLineInfo final
{
    GXColorRGB              _color;
    GXFloat                 _thickness;
    GXVec2                  _startPoint;
    GXVec2                  _endPoint;
    
    eGXImageOverlayType     _overlayType;
};

class GXImageRenderable;
class GXGlyphRenderable;
class GXLineRenderable;
class GXHudSurface final : public GXMemoryInspector, public GXTransform
{
    private:
        GXImageRenderable&          _image;
        GXGlyphRenderable&          _glyph;
        GXLineRenderable&           _line;

        GXUShort                    _width;
        GXUShort                    _height;

        GXCameraOrthographic        _canvasCamera;

        GLuint                      _fbo;
        GXTexture2D                 _canvasTexture;
        GXOpenGLState               _openGLState;

        GXUnlitColorMaterial        _unlitColorMaterial;
        GXUnlitColorMaskMaterial    _unlitColorMaskMaterial;
        GXUnlitTexture2DMaterial    _unlitTexture2DMaterial;

        GXMeshGeometry              _screenQuadMesh;

    public:
        explicit GXHudSurface ( GXUShort width, GXUShort height );
        ~GXHudSurface () override;

        GXVoid Reset ();
        GXVoid Resize ( GXUShort width, GXUShort height );

        GXVoid AddImage ( const GXImageInfo &imageInfo );
        GXVoid AddLine ( const GXLineInfo &lineInfo );
        GXFloat AddText ( const GXPenInfo &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... );

        GXUShort GetWidth () const;
        GXUShort GetHeight () const;

        GXVoid Render ();

    protected:
        GXVoid TransformUpdated () override;

    private:
        GXHudSurface ( const GXHudSurface &other ) = delete;
        GXHudSurface& operator = ( const GXHudSurface &other ) = delete;
};


#endif // GX_HUD_SURFACE
