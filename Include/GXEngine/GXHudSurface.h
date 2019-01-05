// version 1.25

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
    GXTexture2D*            texture;

    GXColorRGB              color;
    GXFloat                 insertX;
    GXFloat                 insertY;
    GXFloat                 insertWidth;
    GXFloat                 insertHeight;

    eGXImageOverlayType     overlayType;
};

struct GXPenInfo final
{
    GXFont*                 font;
    GXColorRGB              color;
    GXFloat                 insertX;
    GXFloat                 insertY;

    eGXImageOverlayType     overlayType;
};

struct GXLineInfo final
{
    GXColorRGB              color;
    GXFloat                 thickness;
    GXVec2                  startPoint;
    GXVec2                  endPoint;
    
    eGXImageOverlayType     overlayType;
};

class GXImageRenderable;
class GXGlyphRenderable;
class GXLineRenderable;
class GXHudSurface final : public GXMemoryInspector, public GXTransform
{
    private:
        GXImageRenderable&          image;
        GXGlyphRenderable&          glyph;
        GXLineRenderable&           line;

        GXUShort                    width;
        GXUShort                    height;

        GXCameraOrthographic        canvasCamera;

        GLuint                      fbo;
        GXTexture2D                 canvasTexture;
        GXOpenGLState               openGLState;

        GXUnlitColorMaterial        unlitColorMaterial;
        GXUnlitColorMaskMaterial    unlitColorMaskMaterial;
        GXUnlitTexture2DMaterial    unlitTexture2DMaterial;

        GXMeshGeometry              screenQuadMesh;

    public:
        explicit GXHudSurface ( GXUShort imageWidth, GXUShort imageHeight );
        ~GXHudSurface () override;

        GXVoid Reset ();
        GXVoid Resize ( GXUShort newWidth, GXUShort newHeight );

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
