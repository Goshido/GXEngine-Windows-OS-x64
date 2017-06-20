//version 1.0

#include <GXEngine/GXUIButton.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


GXUIButton::GXUIButton ( GXWidget* parent ):
GXWidget ( parent )
{
	OnLeftMouseButton = nullptr;
	handler = nullptr;

	isPressed = GX_FALSE;
	isDisabled = GX_FALSE;
	isHighlighted = GX_FALSE;
}

GXUIButton::~GXUIButton ()
{
	//NOTHING
}

GXVoid GXUIButton::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			if ( isDisabled ) return;

			isPressed = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();

			if ( OnLeftMouseButton )
			{
				const GXVec2* position = (const GXVec2*)data;
				OnLeftMouseButton ( handler, this, position->x, position->y, eGXMouseButtonState::Down );
			}
		}
		break;

		case GX_MSG_LMBUP:
		{
			if ( isDisabled ) return;

			isPressed = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();

			if ( !OnLeftMouseButton ) return;

			const GXVec2* position = (const GXVec2*)data;
			OnLeftMouseButton ( handler, this, position->x, position->y, eGXMouseButtonState::Up );
		}
		break;

		case GX_MSG_MOUSE_OVER:
			if ( isDisabled ) return;

			isHighlighted = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_MOUSE_LEAVE:
			if ( isDisabled ) return;

			isHighlighted = GX_FALSE;
			isPressed = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_ENABLE:
			if ( !isDisabled ) return;

			isDisabled = GX_FALSE;
			isHighlighted = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_DISABLE:
			if ( isDisabled ) return;

			isDisabled = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIButton::SetOnLeftMouseButtonCallback ( GXVoid* handler, PFNGXONMOUSEBUTTONPROC callback )
{
	this->handler = handler;
	OnLeftMouseButton = callback;
}

GXVoid GXUIButton::Enable ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_ENABLE, nullptr, 0 );
}

GXVoid GXUIButton::Disable ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_DISABLE, nullptr, 0 );
}

GXVoid GXUIButton::Redraw ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_REDRAW, nullptr, 0 );
}

GXBool GXUIButton::IsPressed () const
{
	return isPressed;
}

GXBool GXUIButton::IsHighlighted () const
{
	return isHighlighted;
}

GXBool GXUIButton::IsDisabled () const
{
	return isDisabled;
}
