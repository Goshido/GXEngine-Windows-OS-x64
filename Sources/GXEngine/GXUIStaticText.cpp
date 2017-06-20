//version 1.0

#include <GXEngine/GXUIStaticText.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXUIMessage.h>
#include <GXCommon/GXStrings.h>


#define GX_UI_DEFAULT_ALIGNMENT		GX_UI_TEXT_ALIGNMENT_LEFT


GXUIStaticText::GXUIStaticText ( GXWidget* parent ):
GXWidget ( parent )
{
	text = nullptr;
	alignment = GX_UI_DEFAULT_ALIGNMENT;
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

GXVoid GXUIStaticText::SetAlignment ( eGXUITextAlignment alignment )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT_ALIGNMENT, &alignment, sizeof ( eGXUITextAlignment ) );
}

const GXWChar* GXUIStaticText::GetText () const
{
	return text;
}

eGXUITextAlignment GXUIStaticText::GetAlignment () const
{
	return alignment;
}
