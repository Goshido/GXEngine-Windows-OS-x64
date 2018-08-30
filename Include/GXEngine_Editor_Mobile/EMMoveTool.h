#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"
#include "EMObjectMaskMaterial.h"
#include "EMMesh.h"
#include <GXEngine/GXInput.h>
#include <GXEngine/GXUnlitColorMaterial.h>


class EMMoveTool final : public EMTool
{
	private:
		GXUByte					mode;
		GXUByte					activeAxis;
		GXVec3					startLocationWorld;
		GXVec3					deltaWorld;

		GXBool					isLMBPressed;
		GXUShort				mouseX;
		GXUShort				mouseY;

		GXFloat					gismoScaleCorrector;

		EMMesh					xAxis;
		EMMesh					xAxisMask;
		EMMesh					yAxis;
		EMMesh					yAxisMask;
		EMMesh					zAxis;
		EMMesh					zAxisMask;
		EMMesh					center;

		EMObjectMaskMaterial	objectMaskMaterial;
		GXUnlitColorMaterial	unlitColorMaterial;

		GXFloat					axisStartParameter;
		GXMat4					gismoRotation;

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

		EMMoveTool ( const EMMoveTool &other ) = delete;
		EMMoveTool& operator = ( const EMMoveTool &other ) = delete;
};


#endif // EM_MOVE_TOOL
