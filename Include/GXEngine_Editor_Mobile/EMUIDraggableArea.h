#ifndef EM_UI_DRAGGABLE_AREA
#define EM_UI_DRAGGABLE_AREA


#include "EMUI.h"
#include <GXEngine/GXUIDraggableArea.h>


class EMUIDraggableArea : public EMUI
{
	private:
		GXUIDragableArea*	widget;

	public:
		EMUIDraggableArea ( EMUI* parent );
		virtual ~EMUIDraggableArea ();

		virtual GXWidget* GetWidget ();
		virtual GXVoid OnDrawMask ();

		GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

		GXVoid Show ();
		GXVoid Hide ();

		GXVoid SetHeaderHeight ( GXFloat headerHeight );
		GXVoid SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback );
};


#endif //EM_UI_DRAGGABLE_AREA
