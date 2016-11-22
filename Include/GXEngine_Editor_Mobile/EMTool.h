#ifndef EM_TOOL
#define EM_TOOL


#include "EMActor.h"
#include <GXEngine/GXInput.h>


class EMTool
{
	protected:
		EMActor*		actor;

	public:
		EMTool ();
		virtual ~EMTool ();

		virtual GXVoid Bind ();
		virtual GXVoid SetActor ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnViewerTransformChanged ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();

		virtual GXVoid OnMouseMove ( const GXVec2 &mousePosition );
		virtual GXVoid OnMouseButton ( EGXInputMouseFlags mouseflags );
		virtual GXVoid OnMouseWheel ( GXInt steps );
};


#endif //EM_TOOL
