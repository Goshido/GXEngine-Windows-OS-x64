// version 1.0

#ifndef GX_UI_MESSAGE
#define GX_UI_MESSAGE


// Name												Message ID		Parameter type					Description	
#define GX_MSG_LMBDOWN								0u				// GXVec2						x, y - world mouse position
#define GX_MSG_LMBUP								1u				// GXVec2						x, y - world mouse position
#define GX_MSG_MMBDOWN								2u				// GXVec2						x, y - world mouse position
#define GX_MSG_MMBUP								3u				// GXVec2						x, y - world mouse position
#define GX_MSG_RMBDOWN								4u				// GXVec2						x, y - world mouse position
#define GX_MSG_RMBUP								5u				// GXVec2						x, y - world mouse position
#define GX_MSG_SCROLL								6u				// GXVec3						x, y - world mouse position. z - scroll
#define GX_MSG_MOUSE_MOVE							7u				// GXVec2						x, y - world mouse position
#define GX_MSG_MOUSE_OVER							8u				// GXVec2						x, y - world mouse position
#define GX_MSG_MOUSE_LEAVE							9u				// GXVec2						x, y - world mouse position
#define GX_MSG_ENABLE								10u				// none							none
#define GX_MSG_DISABLE								11u				// none							none
#define GX_MSG_RESIZE								12u				// GXAABB						new local bounds of widget
#define GX_MSG_REDRAW								13u				// none							none
#define GX_MSG_HIDE									14u				// none							none
#define GX_MSG_SHOW									15u				// none							none
#define GX_MSG_DRAGGABLE_AREA_SET_HEADER_HEIGHT		16u				// GXFloat						new header height
#define GX_MSG_DRAGGABLE_AREA_SET_BORDER_THICKNESS	17u				// GXFloat						new border thickness
#define GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_WIDTH		18u				// GXFloat						new minimum width
#define GX_MSG_DRAGGABLE_AREA_SET_MINIMUM_HEIGHT	19u				// GXFloat						new minimum height
#define GX_MSG_DRAG									20u				// GXVec2						x, y - global mouse delta
#define GX_MSG_FOREGROUND							21u				// none							none
#define GX_MSG_SET_TEXT								22u				// const GXWChar*				new text
#define GX_MSG_CLEAR_TEXT							23u				// none							none
#define GX_MSG_SET_TEXT_ALIGNMENT					24u				// eGXUITextAlignment			see eGXUIStaticTextAlignment
#define GX_MSG_SET_TEXT_COLOR						25u				// GXVec4						text color
#define GX_MSG_LIST_BOX_ADD_ITEM					26u				// GXVoid*						item content
#define GX_MSG_LIST_BOX_ADD_ITEMS					27u				// GXUIListBoxRawItem*			item list
#define GX_MSG_LIST_BOX_REMOVE_ITEM					28u				// GXUInt						item index
#define GX_MSG_LIST_BOX_REMOVE_ALL_ITEMS			29u				// none							none
#define GX_MSG_LIST_BOX_SET_VIEWPORT_OFFSET			30u				// GXFloat						new view port offset
#define GX_MSG_LIST_BOX_SET_ITEM_HEIGHT				31u				// GXFloat						new item height
#define GX_MSG_DOUBLE_CLICK							32u				// GXVec2						x, y - world mouse position
#define GX_MSG_EDIT_BOX_SET_FONT					33u				// GXUIEditBoxFontInfo			see GXUIEditBoxFontInfo
#define GX_MSG_ADD_SYMBOL							34u				// GXWChar						symbol
#define GX_MSG_EDIT_BOX_SET_TEXT_LEFT_OFFSET		35u				// GXFloat						text left offset
#define GX_MSG_EDIT_BOX_SET_TEXT_RIGHT_OFFSET		36u				// GXFloat						text right offset
#define GX_MSG_POPUP_ADD_ITEM						37u				// GXUIPopupItem				see GXUIPopupItem
#define GX_MSG_POPUP_SET_ITEM_HEIGHT				38u				// GXFloat						item height
#define GX_MSG_POPUP_ENABLE_ITEM					39u				// GXUByte						item index
#define GX_MSG_POPUP_DISABLE_ITEM					40u				// GXUByte						item index
#define GX_MSG_POPUP_CLOSED							41u				// GXUIPopup*					closed popup
#define GX_MSG_POPUP_SHOW							42u				// GXWidget*					owner widget
#define GX_MSG_MENY_ADD_ITEM						43u				// GXUIMenuExtItem				see GXUIMenuExtItem
#define GX_MSG_KEY_DOWN								44u				// GXInt						virtual key code
#define GX_MSG_KEY_UP								45u				// GXInt						virtual key code


#endif // GX_UI_MESSAGE
