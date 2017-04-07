//version 1.18

#ifndef GX_HUD_SURFACE
#define GX_HUD_SURFACE


#include "GXRenderable.h"
#include "GXCameraOrthographic.h"
#include "GXTransform.h"
#include "GXMeshGeometry.h"
#include "GXTexture.h"
#include "GXFont.h"
#include "GXUnlitColorMaterial.h"
#include "GXUnlitColorMaskMaterial.h"
#include "GXUnlitTexture2DMaterial.h"


enum class eGXImageOverlayType
{
	AlphaTransparency,
	AlphaTransparencyPreserveAlpha,
	AlphaAdd,
	AlphaMultiply,
	SimpleReplace
};

struct GXImageInfo
{
	GXTexture*			texture;

	GXVec4				color;
	GXFloat				insertX;
	GXFloat				insertY;
	GXFloat				insertWidth;
	GXFloat				insertHeight;

	eGXImageOverlayType	overlayType;
};

struct GXPenInfo
{
	GXFont*				font;
	GXVec4				color;
	GXFloat 			insertX;
	GXFloat				insertY;

	eGXImageOverlayType	overlayType;
};

struct GXLineInfo
{
	GXVec4				color;
	GXFloat				thickness;
	GXVec2				startPoint;
	GXVec2				endPoint;
	
	eGXImageOverlayType	overlayType;
};

class GXImageRenderable;
class GXGlyphRenderable;
class GXLineRenderable;
class GXHudSurface : public GXTransform
{
	private:
		GLuint						fbo;
		GXTexture					canvasTexture;
		GLuint						sampler;

		GXUnlitColorMaterial		unlitColorMaterial;
		GXUnlitColorMaskMaterial	unlitColorMaskMaterial;
		GXUnlitTexture2DMaterial	unlitTexture2DMaterial;

		GXMeshGeometry				screenQuadMesh;

		GXImageRenderable*			image;
		GXGlyphRenderable*			glyph;
		GXLineRenderable*			line;

		GXCameraOrthographic		canvasCamera;

		GXUShort					width;
		GXUShort					height;

		GXBool						enableSmooth;

	public:
		explicit GXHudSurface ( GXUShort width, GXUShort height );
		~GXHudSurface () override;
		
		GXVoid Reset ();

		GXVoid AddImage ( const GXImageInfo &imageInfo );
		GXVoid AddLine ( const GXLineInfo &lineInfo );
		GXFloat AddText ( const GXPenInfo &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... );

		GXUShort GetWidth () const;
		GXUShort GetHeight () const;

		GXVoid Render ();

	protected:
		GXVoid TransformUpdated () override;
};


#endif //GX_HUD_SURFACE
