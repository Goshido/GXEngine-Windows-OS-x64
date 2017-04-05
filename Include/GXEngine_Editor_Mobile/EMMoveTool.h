#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"
#include "EMObjectMaskMaterial.h"
#include "EMMesh.h"
#include <GXEngine/GXInput.h>


class EMMoveTool : public EMTool
{
	private:
		GXVec3					startLocationWorld;
		GXMat4					gismoRotation;
		GXVec3					deltaWorld;
		GXFloat					gismoScaleCorrector;
		GXFloat					axisStartParameter;
		GXUByte					mode;
		GXUByte					activeAxis;

		GXBool					isLMBPressed;
		GXUShort				mouseX;
		GXUShort				mouseY;

		EMMesh					xAxis;
		EMMesh					xAxisMask;
		EMMesh					yAxis;
		EMMesh					yAxisMask;
		EMMesh					zAxis;
		EMMesh					zAxisMask;
		EMMesh					center;

		GXShaderProgram			colorProgram;
		EMObjectMaskMaterial	objectMaskMaterial;

		GLint					clr_mod_view_proj_matLocation;
		GLint					clr_colorLocation;

		GXBool					isDeleted;

	public:
		EMMoveTool ();
		GXVoid Delete ();

		GXVoid Bind () override;
		GXVoid SetActor ( EMActor* actor ) override;
		GXVoid UnBind () override;

		GXVoid OnViewerTransformChanged () override;

		GXVoid OnDrawHudColorPass () override;
		GXVoid OnDrawHudMaskPass () override;

		GXVoid OnMouseMove ( const GXVec2 &mousePosition ) override;
		GXVoid OnMouseButton ( EGXInputMouseFlags mouseflags ) override;

		GXVoid SetLocalMode ();
		GXVoid SetWorldMode ();

	protected:
		~EMMoveTool () override;

	private:
		GXVoid InitGraphicResources ();

		GXVoid SetMode ( GXUByte mode );

		GXVoid OnMoveActor ();

		GXVoid GetAxis ( GXVec3& axisView );
		GXVoid GetRayPerspective ( GXVec3 &rayView );
		GXFloat GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView );
		GXFloat GetScaleCorrector ( const GXVec3 &axisLocationView, const GXVec3 &deltaView );

		GXVoid UpdateMeshTransform ( EMMesh &mesh );

		static GXVoid GXCALL OnObject ( GXUPointer object );
};


#endif //EM_MOVE_TOOL
