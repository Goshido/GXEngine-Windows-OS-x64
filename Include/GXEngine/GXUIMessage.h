// version 1.4

#ifndef GX_UI_MESSAGE
#define GX_UI_MESSAGE


#include <GXCommon/GXTypes.h>


enum class eGXUIMessage : GXUInt
{
    // Name                                 Message ID          Parameter type                  Description    
    LMBDown                                 = 0u,               // GXVec2                       x, y - world mouse position
    LMBUp                                   = 1u,               // GXVec2                       x, y - world mouse position
    MMBDown                                 = 3u,               // GXVec2                       x, y - world mouse position
    MMBUp                                   = 2u,               // GXVec2                       x, y - world mouse position
    RMBDown                                 = 4u,               // GXVec2                       x, y - world mouse position
    RMBUp                                   = 5u,               // GXVec2                       x, y - world mouse position
    Scroll                                  = 6u,               // GXVec3                       x, y - world mouse position. z - scroll
    MouseMove                               = 7u,               // GXVec2                       x, y - world mouse position
    MouseOver                               = 8u,               // GXVec2                       x, y - world mouse position
    MouseLeave                              = 9u,               // GXVec2                       x, y - world mouse position
    Enable                                  = 10u,              // none                         none
    Disable                                 = 11u,              // none                         none
    Resize                                  = 12u,              // GXAABB                       new local bounds of widget
    Redraw                                  = 13u,              // none                         none
    Hide                                    = 14u,              // none                         none
    Show                                    = 15u,              // none                         none
    DraggableAreaSetHeaderHeght             = 16u,              // GXFloat                      new header height
    DraggableAreaSetBorderThickness         = 17u,              // GXFloat                      new border thickness
    DraggableAreaSetMinimumWidth            = 18u,              // GXFloat                      new minimum width
    DraggableAreaSetMinimumHeight           = 19u,              // GXFloat                      new minimum height
    Drag                                    = 20u,              // GXVec2                       x, y - global mouse delta
    Foreground                              = 21u,              // none                         none
    SetText                                 = 22u,              // const GXUTF16*               new text
    ClearText                               = 23u,              // none                         none
    SetTextAlignment                        = 24u,              // eGXUITextAlignment           see eGXUIStaticTextAlignment
    SetTextColor                            = 25u,              // GXVec4                       text color
    ListBoxAddItem                          = 26u,              // GXVoid*                      item content
    ListBoxAddItems                         = 27u,              // GXUIListBoxRawItem*          item list
    ListBoxRemoveItem                       = 28u,              // GXUInt                       item index
    ListBoxRemoveAllItems                   = 29u,              // none                         none
    ListBoxSetViewportOffset                = 30u,              // GXFloat                      new view port offset
    ListBoxSetItemHeight                    = 31u,              // GXFloat                      new item height
    DoubleClick                             = 32u,              // GXVec2                       x, y - world mouse position
    EditBoxSetFont                          = 33u,              // GXUIEditBoxFontInfo          see GXUIEditBoxFontInfo
    AddSymbol                               = 34u,              // GXWChar                      symbol
    EditBoxSetTextLeftOffset                = 35u,              // GXFloat                      text left offset
    EditBoxSetTextRightOffset               = 36u,              // GXFloat                      text right offset
    PopupAddItem                            = 37u,              // GXUIPopupItem                see GXUIPopupItem
    PopupSetItemHeight                      = 38u,              // GXFloat                      item height
    PopupEnableItem                         = 39u,              // GXUByte                      item index
    PopupDisableItem                        = 40u,              // GXUByte                      item index
    PopupClosed                             = 41u,              // GXUIPopup*                   closed popup
    PopupShow                               = 42u,              // GXWidget*                    owner widget
    MenuAddItem                             = 43u,              // GXUIMenuExtItem              see GXUIMenuExtItem
    KeyDown                                 = 44u,              // GXInt                        virtual key code
    KeyUp                                   = 45u               // GXInt                        virtual key code
};

#endif // GX_UI_MESSAGE
