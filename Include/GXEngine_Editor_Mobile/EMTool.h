#ifndef EM_TOOL
#define EM_TOOL


#include "EMActor.h"
#include <GXEngine/GXUIInput.h>
#include <GXEngine/GXInput.h>


class EMTool
{
	protected:
		EMActor*			actor;

	private:
		static EMTool*		activeTool;

	public:
		EMTool ();
		virtual ~EMTool ();

		virtual GXVoid Bind ();
		virtual GXVoid SetActor ( EMActor* newActor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnViewerTransformChanged ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();

		virtual GXBool OnLeftMouseButtonDown ( GXFloat x, GXFloat y );
		virtual GXBool OnLeftMouseButtonUp ( GXFloat x, GXFloat y );
		virtual GXBool OnMouseMove ( GXFloat x, GXFloat y );
		virtual GXBool OnMouseScroll ( GXFloat x, GXFloat y, GXFloat scroll );
		virtual GXBool OnKeyDown ( GXInt keyCode );
		virtual GXBool OnKeyUp ( GXInt keyCode );

		virtual GXBool OnObject ( GXVoid* object ) = 0;

		static EMTool* GXCALL GetActiveTool ();
		static GXVoid GXCALL SetActiveTool ( EMTool* tool );

	private:
		EMTool ( const EMTool &other ) = delete;
		EMTool& operator = ( const EMTool &other ) = delete;
};


#endif // EM_TOOL
