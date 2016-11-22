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
#define GX_MSG_MENU_ADD_ITEM						14				//none							none
#define GX_MSG_MENU_RESIZE_ITEM						15				//GXUIMenuResizeItem			see GXUIMenuResizeItem
#define GX_MSG_SUBMENU_ADD_ITEM						16				//PFNGXONMOUSEBUTTONPROC		item callback function
#define GX_MSG_SUBMENU_SET_HEIGHT					17				//GXFloat						new height
#define GX_MSG_HIDE									18				//none							none
#define GX_MSG_SHOW									19				//none							none
#define GX_MSG_MOVABLE_AREA_SET_HEADER_HEIGHT		20				//GXFloat						new header height
#define GX_MSG_MOVABLE_AREA_SET_BORDER_THICKNESS	21				//GXFloat						new border thickness
#define GX_MSG_DRAG									22				//GXVec2						x, y - global mouse delta
#define GX_MSG_FOREGROUND							23				//none							none
#define GX_MSG_SET_TEXT								24				//const GXWChar*				new text
#define GX_MSG_CLEAR_TEXT							25				//none							none
#define GX_MSG_SET_TEXT_ALIGNMENT					26				//eGXUITextAlignment			see eGXUIStaticTextAlignment
#define GX_MSG_LIST_BOX_ADD_ITEM					27				//GXVoid*						item content
#define GX_MSG_LIST_BOX_ADD_ITEMS					28				//GXUIListBoxRawItem*			item list
#define GX_MSG_LIST_BOX_REMOVE_ITEM					29				//GXUInt						item index
#define GX_MSG_LIST_BOX_REMOVE_ALL_ITEMS			30				//none							none
#define GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET			31				//GXFloat						new view port offset
#define GX_MSG_LIST_BOX_SET_ITEM_HEIGHT				32				//GXFloat						new item height
#define GX_MSG_DOUBLE_CLICK							33				//GXVec2						x, y - world mouse position
#define GX_MSG_EDIT_BOX_SET_FONT					34				//GXUIEditBoxFontInfo			see GXUIEditBoxFontInfo
#define GX_MSG_ADD_SYMBOL							35				//GXWChar						symbol
#define GX_MSG_EDIT_BOX_SET_TEXT_LEFT_OFFSET		36				//GXFloat						text left offset
#define GX_MSG_EDIT_BOX_SET_TEXT_RIGHT_OFFSET		37				//GXFloat						text right offset


#endif //GX_UI_MESSAGE
