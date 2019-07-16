#ifndef EM_UI_DRAGGABLE_AREA
#define EM_UI_DRAGGABLE_AREA


#include "EMUI.h"
#include <GXEngine/GXUIDraggableArea.h>


class EMUIDraggableArea final : public EMUI
{
    private:
        GXUIDragableArea*       _widget;

    public:
        explicit EMUIDraggableArea ( EMUI* parent );
        ~EMUIDraggableArea () override;

        GXWidget* GetWidget () const override;

        GXVoid Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height );

        GXVoid Show ();
        GXVoid Hide ();

        GXVoid SetHeaderHeight ( GXFloat headerHeight );
        GXVoid SetMinimumWidth ( GXFloat width );
        GXVoid SetMinimumHeight ( GXFloat height );
        GXVoid SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback );

    private:
        EMUIDraggableArea () = delete;
        EMUIDraggableArea ( const EMUIDraggableArea &other ) = delete;
        EMUIDraggableArea& operator = ( const EMUIDraggableArea &other ) = delete;
};


#endif // EM_UI_DRAGGABLE_AREA
