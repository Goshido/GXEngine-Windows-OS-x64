//version 1.0

#include <GXEngine/GXUIStaticText.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXCommon/GXStrings.h>


#define GX_UI_DEFAULT_ALIGNMENT		eGXUITextAlignment::Left

#define DEFAULT_TEXT_COLOR_R		115
#define DEFAULT_TEXT_COLOR_G		185
#define DEFAULT_TEXT_COLOR_B		0
#define DEFAULT_TEXT_COLOR_A		255


GXUIStaticText::GXUIStaticText ( GXWidget* parent ):
GXWidget ( parent )
{
	text = nullptr;
	alignment = GX_UI_DEFAULT_ALIGNMENT;
	GXColorToVec4 ( textColor, DEFAULT_TEXT_COLOR_R, DEFAULT_TEXT_COLOR_G, DEFAULT_TEXT_COLOR_B, DEFAULT_TEXT_COLOR_A );
}

GXUIStaticText::~GXUIStaticText ()
{
	GXSafeFree ( text );
}

GXVoid GXUIStaticText::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_SET_TEXT:
			GXSafeFree ( text );
			GXWcsclone ( &text, (const GXWChar*)data );

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_CLEAR_TEXT:
			GXSafeFree ( text );

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_SET_TEXT_COLOR:
			memcpy ( &textColor, data, sizeof ( GXVec4 ) );

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_SET_TEXT_ALIGNMENT:
		{
			const eGXUITextAlignment* newAlignment = (const eGXUITextAlignment*)data;
			alignment = *newAlignment;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIStaticText::SetText ( const GXWChar* text )
{
	if ( text )
		GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT, text, ( GXWcslen ( text ) + 1 ) * sizeof ( GXWChar ) );
	else
		GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_CLEAR_TEXT, 0, 0 );
}

GXVoid GXUIStaticText::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXVec4 c;
	GXColorToVec4 ( c, red, green, blue, alpha );
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT_COLOR, &c, sizeof ( GXVec4 ) );
}

GXVoid GXUIStaticText::SetAlignment ( eGXUITextAlignment alignment )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT_ALIGNMENT, &alignment, sizeof ( eGXUITextAlignment ) );
}

const GXWChar* GXUIStaticText::GetText () const
{
	return text;
}

const GXVec4& GXUIStaticText::GetTextColor () const
{
	return textColor;
}

eGXUITextAlignment GXUIStaticText::GetAlignment () const
{
	return alignment;
}
