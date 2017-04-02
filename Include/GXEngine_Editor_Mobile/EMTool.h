#ifndef EM_TOOL
#define EM_TOOL


#include "EMActor.h"
#include <GXEngine/GXUIInput.h>
#include <GXEngine/GXInput.h>


class EMTool
{
	private:
		static EMTool*		activeTool;
		static GXUIInput*	inputWidget;
		static GXInt		inputWidgetRef;

	protected:
		EMActor*			actor;

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
		virtual GXVoid OnDoubleClick ( const GXVec2 &mousePosition );

		static GXVoid GXCALL SetActiveTool ( EMTool* tool );

	private:
		static GXVoid GXCALL OnLMBDown ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnLMBUp ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMMBDown ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMMBUp ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnRMBDown ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnRMBUp ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );

		static GXVoid GXCALL OnDoubleClick ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMouseMove ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMouseScroll ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y, GXFloat scroll );
};


#endif //EM_TOOL
