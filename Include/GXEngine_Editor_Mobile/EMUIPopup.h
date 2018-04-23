#ifndef EM_UI_POPUP
#define EM_UI_POPUP


#include "EMUI.h"
#include <GXEngine/GXUIPopup.h>


class EMUIPopup : public EMUI
{
	private:
		GXUIPopup*		widget;

	public:
		explicit EMUIPopup ( EMUI* parent );
		~EMUIPopup () override;

		GXWidget* GetWidget () const override;

		GXVoid AddItem ( const GXWChar* name, GXVoid* handler, PFNGXONUIPOPUPACTIONPROC action );

		GXVoid EnableItem ( GXUByte itemIndex );
		GXVoid DisableItem ( GXUByte itemIndex );

		GXVoid SetLocation ( GXFloat x, GXFloat y );

		GXVoid Show ( EMUI* owner );
};


#endif // EM_UI_POPUP
