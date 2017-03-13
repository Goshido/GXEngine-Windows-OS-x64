#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"
#include <GXEngine/GXInput.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXEngine/GXShaderProgram.h>


class EMMoveTool : public EMTool
{
	private:
		GXVec3				startLocationWorld;
		GXMat4				gismoRotation;
		GXVec3				deltaWorld;
		GXFloat				gismoScaleCorrector;
		GXFloat				axisStartParameter;
		GXUByte				mode;
		GXUByte				activeAxis;

		GXBool				isLMBPressed;
		GXUShort			mouseX;
		GXUShort			mouseY;

		GXVAOInfo			xAxis;
		GXVAOInfo			xAxisMask;
		GXVAOInfo			yAxis;
		GXVAOInfo			yAxisMask;
		GXVAOInfo			zAxis;
		GXVAOInfo			zAxisMask;
		GXVAOInfo			center;

		GXShaderProgram		colorProgram;
		GXShaderProgram		maskProgram;

		GLint				clr_mod_view_proj_matLocation;
		GLint				clr_colorLocation;
		GLint				msk_mod_view_proj_matLocation;

		GXBool				isDeleted;

	public:
		EMMoveTool ();
		GXVoid Delete ();

		virtual GXVoid Bind ();
		virtual GXVoid SetActor ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnViewerTransformChanged ();

		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();

		virtual GXVoid OnMouseMove ( const GXVec2 &mousePosition );
		virtual GXVoid OnMouseButton ( EGXInputMouseFlags mouseflags );

		GXVoid SetLocalMode ();
		GXVoid SetWorldMode ();

	protected:
		virtual ~EMMoveTool ();

	private:
		GXVoid Load3DModels ();
		GXVoid InitUniforms ();

		GXVoid SetMode ( GXUByte mode );

		GXVoid OnMoveActor ();

		GXVoid GetAxis ( GXVec3& axisView );
		GXVoid GetRayPerspective ( GXVec3 &rayView );
		GXFloat GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView );
		GXFloat GetScaleCorrector ( const GXVec3 &axisLocationView, const GXVec3 &deltaView );

		static GXVoid GXCALL OnObject ( GXUPointer object );
};


#endif //EM_MOVE_TOOL
