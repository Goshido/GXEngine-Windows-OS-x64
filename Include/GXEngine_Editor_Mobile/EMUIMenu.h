#ifndef EM_UI_MENU
#define EM_UI_MENU


#include "EMUI.h"
#include "EMUIPopup.h"
#include <GXEngine/GXUIMenu.h>


class EMUIMenu : public EMUI
{
	private:
		GXUIMenu*	widget;

	public:
		explicit EMUIMenu ( EMUI* parent );
		~EMUIMenu () override;

		GXWidget* GetWidget () const override;

		GXVoid AddItem ( const GXWChar* name, EMUIPopup* popup );
		GXVoid SetLocation ( GXFloat leftBottomX, GXFloat leftBottomY );
		GXFloat GetHeight () const;
};


#endif // EM_UI_MENU
