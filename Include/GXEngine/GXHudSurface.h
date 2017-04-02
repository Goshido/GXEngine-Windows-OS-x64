//version 1.18

#ifndef GX_HUD_SURFACE
#define GX_HUD_SURFACE


#include "GXRenderable.h"
#include "GXMeshGeometry.h"
#include "GXTexture.h"
#include "GXShaderProgram.h"
#include "GXFont.h"


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

class GXHudSurface : public GXRenderable
{
	protected:
		GLuint				fbo;
		GXTexture			texture;
		GLuint				sampler;

		GXMeshGeometry		screenQuad;
		GXShaderProgram		imageShaderProgram;
		GLint				imageModelViewProjectionMatrixLocation;
		GLint				imageColorLocation;

		GXMeshGeometry		glyphMesh;
		GXShaderProgram		glyphShaderProgram;
		GLint				glyphModelViewProjectionMatrixLocation;
		GLint				glyphColorLocation;

		GXMeshGeometry		lineMesh;
		GXShaderProgram		lineShaderProgram;
		GLint				lineModelViewProjectionMatrixLocation;
		GLint				lineColorLocation;

		GXMeshGeometry		surfaceMesh;

		GXMat4				projectionMatrix;

		GXUShort			width;
		GXUShort			height;

		GXBool				enableSmooth;

	public:
		explicit GXHudSurface ( GXUShort width, GXUShort height );
		~GXHudSurface () override;
		
		GXVoid Reset ();

		GXVoid AddImage ( const GXImageInfo &imageInfo );
		GXVoid AddLine ( const GXLineInfo &lineInfo );
		GXFloat AddText ( const GXPenInfo &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... );

		GXUShort GetWidth () const;
		GXUShort GetHeight () const;

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;

		GXVoid UpdateGlyphGeometry ( const GXVec2 &min, const GXVec2 &max );
		GXVoid UpdateLineGeometry ( const GXVec2 &start, const GXVec2 &end );
};


#endif //GX_HUD_SURFACE
