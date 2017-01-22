//version 1.0

#ifndef GX_UI_MESSAGE
#define GX_UI_MESSAGE


//Name												Message ID		Parameter type					Description	
#define GX_MSG_LMBDOWN								0				//GXVec2						x, y - world mouse position
#define GX_MSG_LMBUP								1				//GXVec2						x, y - world mouse position
#define GX_MSG_MMBDOWN								2				//GXVec2						x, y - world mouse position
#define GX_MSG_MMBUP								3				//GXVec2						x, y - world mouse position
#define GX_MSG_RMBDOWN								4				//GXVec2						x, y - world mouse position
#define GX_MSG_RMBUP								5				//GXVec2						x, y - world mouse position
#define GX_MSG_SCROLL								6				//GXVec3						x, y - world mouse position. z - scroll
#define GX_MSG_MOUSE_MOVE							7				//GXVec2						x, y - world mouse position
#define GX_MSG_MOUSE_OVER							8				//GXVec2						x, y - world mouse position
#define GX_MSG_MOUSE_LEAVE							9				//GXVec2						x, y - world mouse position
#define GX_MSG_ENABLE								10				//none							none
#define GX_MSG_DISABLE								11				//none							none
#define GX_MSG_RESIZE								12				//GXAABB						new local bounds of widget
#define GX_MSG_REDRAW								13				//none							none
#define GX_MSG_HIDE									14				//none							none
#define GX_MSG_SHOW									15				//none							none
#define GX_MSG_MOVABLE_AREA_SET_HEADER_HEIGHT		16				//GXFloat						new header height
#define GX_MSG_MOVABLE_AREA_SET_BORDER_THICKNESS	17				//GXFloat						new border thickness
#define GX_MSG_DRAG									18				//GXVec2						x, y - global mouse delta
#define GX_MSG_FOREGROUND							19				//none							none
#define GX_MSG_SET_TEXT								20				//const GXWChar*				new text
#define GX_MSG_CLEAR_TEXT							21				//none							none
#define GX_MSG_SET_TEXT_ALIGNMENT					22				//eGXUITextAlignment			see eGXUIStaticTextAlignment
#define GX_MSG_LIST_BOX_ADD_ITEM					23				//GXVoid*						item content
#define GX_MSG_LIST_BOX_ADD_ITEMS					24				//GXUIListBoxRawItem*			item list
#define GX_MSG_LIST_BOX_REMOVE_ITEM					25				//GXUInt						item index
#define GX_MSG_LIST_BOX_REMOVE_ALL_ITEMS			26				//none							none
#define GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET			27				//GXFloat						new view port offset
#define GX_MSG_LIST_BOX_SET_ITEM_HEIGHT				28				//GXFloat						new item height
#define GX_MSG_DOUBLE_CLICK							29				//GXVec2						x, y - world mouse position
#define GX_MSG_EDIT_BOX_SET_FONT					30				//GXUIEditBoxFontInfo			see GXUIEditBoxFontInfo
#define GX_MSG_ADD_SYMBOL							31				//GXWChar						symbol
#define GX_MSG_EDIT_BOX_SET_TEXT_LEFT_OFFSET		32				//GXFloat						text left offset
#define GX_MSG_EDIT_BOX_SET_TEXT_RIGHT_OFFSET		33				//GXFloat						text right offset
#define GX_MSG_POPUP_ADD_ITEM						34				//PFNGXONUIPOPUPACTIONPROC		action callback
#define GX_MSG_POPUP_SET_ITEM_HEIGHT				35				//GXFloat						item height
#define GX_MSG_POPUP_ENABLE_ITEM					36				//GXUByte						item index
#define GX_MSG_POPUP_DISABLE_ITEM					37				//GXUByte						item index
#define GX_MSG_POPUP_CLOSED							38				//GXUIPopup*					closed popup
#define GX_MSG_POPUP_SHOW							39				//GXWidget*						owner widget
#define GX_MSG_MENY_ADD_ITEM						40				//GXUIMenuExtItem				see GXUIMenuExtItem


#endif //GX_UI_MESSAGE
