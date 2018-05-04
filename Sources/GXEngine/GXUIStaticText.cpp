// version 1.1

#include <GXEngine/GXUIStaticText.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXCommon/GXStrings.h>


#define DEFAULT_UI_ALIGNMENT		eGXUITextAlignment::Left

#define DEFAULT_TEXT_COLOR_R		115u
#define DEFAULT_TEXT_COLOR_G		185u
#define DEFAULT_TEXT_COLOR_B		0u
#define DEFAULT_TEXT_COLOR_A		255u

//---------------------------------------------------------------------------------------------------------------------

GXUIStaticText::GXUIStaticText ( GXWidget* parent ):
	GXWidget ( parent ),
	text ( nullptr ),
	textColor ( static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_R ), static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_G ), static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_B ), static_cast<GXUByte> ( DEFAULT_TEXT_COLOR_A ) ),
	alignment ( DEFAULT_UI_ALIGNMENT )
{
	// NOTHING
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
			GXWcsclone ( &text, static_cast<const GXWChar*> ( data ) );

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
			const eGXUITextAlignment* newAlignment = static_cast<const eGXUITextAlignment*> ( data );
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

GXVoid GXUIStaticText::SetText ( const GXWChar* newText )
{
	if ( newText )
		GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT, newText, ( GXWcslen ( newText ) + 1 ) * sizeof ( GXWChar ) );
	else
		GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_CLEAR_TEXT, nullptr, 0u );
}

GXVoid GXUIStaticText::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXColorRGB c;
	c.From ( red, green, blue, alpha );
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT_COLOR, &c, sizeof ( GXColorRGB ) );
}

GXVoid GXUIStaticText::SetAlignment ( eGXUITextAlignment newAlignment )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT_ALIGNMENT, &newAlignment, sizeof ( eGXUITextAlignment ) );
}

const GXWChar* GXUIStaticText::GetText () const
{
	return text;
}

const GXColorRGB& GXUIStaticText::GetTextColor () const
{
	return textColor;
}

eGXUITextAlignment GXUIStaticText::GetAlignment () const
{
	return alignment;
}
