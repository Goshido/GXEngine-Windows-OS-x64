#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"
#include "EMObjectMaskMaterial.h"
#include "EMMesh.h"
#include <GXEngine/GXInput.h>
#include <GXEngine/GXUnlitColorMaterial.h>


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

		EMObjectMaskMaterial	objectMaskMaterial;
		GXUnlitColorMaterial	unlitColorMaterial;

	public:
		EMMoveTool ();
		~EMMoveTool () override;

		GXVoid Bind () override;
		GXVoid SetActor ( EMActor* currentActor ) override;
		GXVoid UnBind () override;

		GXVoid OnViewerTransformChanged () override;

		GXVoid OnDrawHudColorPass () override;
		GXVoid OnDrawHudMaskPass () override;

		GXBool OnMouseMove ( GXFloat x, GXFloat y ) override;
		GXBool OnLeftMouseButtonDown ( GXFloat x, GXFloat y ) override;
		GXBool OnLeftMouseButtonUp ( GXFloat x, GXFloat y ) override;

		GXBool OnObject ( GXVoid* object ) override;

		GXVoid SetLocalMode ();
		GXVoid SetWorldMode ();

	private:
		GXVoid UpdateModeMode ();

		GXVoid OnMoveActor ();

		GXVoid GetAxis ( GXVec3& axisView );
		GXVoid GetRayPerspective ( GXVec3 &rayView );
		GXFloat GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView );
		GXFloat GetScaleCorrector ( const GXVec3 &axisLocationView, const GXVec3 &deltaView );

		GXVoid UpdateMeshTransform ( EMMesh &mesh );
};


#endif //EM_MOVE_TOOL
