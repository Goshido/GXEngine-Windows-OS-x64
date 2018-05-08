// version 1.2

#include <GXEngine/GXUIButton.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>


GXUIButton::GXUIButton ( GXWidget* parent ):
	GXWidget ( parent ),
	OnLeftMouseButton ( nullptr ),
	handler ( nullptr ),
	isPressed ( GX_FALSE ),
	isDisabled ( GX_FALSE ),
	isHighlighted ( GX_FALSE )
{
	// NOTHING
}

GXUIButton::~GXUIButton ()
{
	// NOTHING
}

GXVoid GXUIButton::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
	if ( message == eGXUIMessage::LMBDown )
	{
		if ( isDisabled ) return;

		isPressed = GX_TRUE;

		if ( renderer )
			renderer->OnUpdate ();

		if ( !OnLeftMouseButton ) return;

		const GXVec2* position = static_cast<const GXVec2*> ( data );
		OnLeftMouseButton ( handler, *this, position->GetX (), position->GetY (), eGXMouseButtonState::Down );

		return;
	}

	if ( message == eGXUIMessage::LMBUp )
	{
		if ( isDisabled ) return;

		isPressed = GX_FALSE;

		if ( renderer )
			renderer->OnUpdate ();

		if ( !OnLeftMouseButton ) return;

		const GXVec2* position = static_cast<const GXVec2*> ( data );
		OnLeftMouseButton ( handler, *this, position->GetX (), position->GetY (), eGXMouseButtonState::Up );

		return;
	}

	if ( message == eGXUIMessage::MouseOver )
	{
		if ( isDisabled ) return;

		isHighlighted = GX_TRUE;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::MouseLeave )
	{
		if ( isDisabled ) return;

		isHighlighted = GX_FALSE;
		isPressed = GX_FALSE;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::Enable )
	{
		if ( !isDisabled ) return;

		isDisabled = GX_FALSE;
		isHighlighted = GX_FALSE;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::Disable )
	{
		if ( isDisabled ) return;

		isDisabled = GX_TRUE;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::Redraw )
	{
		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	GXWidget::OnMessage ( message, data );
}

GXVoid GXUIButton::SetOnLeftMouseButtonCallback ( GXVoid* handlerObject, PFNGXONMOUSEBUTTONPROC callback )
{
	handler = handlerObject;
	OnLeftMouseButton = callback;
}

GXVoid GXUIButton::Enable ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Enable, nullptr, 0u );
}

GXVoid GXUIButton::Disable ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Disable, nullptr, 0u );
}

GXVoid GXUIButton::Redraw ()
{
	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Redraw, nullptr, 0u );
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
