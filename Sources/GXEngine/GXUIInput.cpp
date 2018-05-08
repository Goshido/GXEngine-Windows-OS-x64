// version 1.3

#include <GXEngine/GXUIInput.h>
#include <GXEngine/GXUIMessage.h>


GXUIInput::GXUIInput ( GXWidget* parent, GXBool isNeedRegister ):
	GXWidget ( parent, isNeedRegister ),
	OnLMBDown ( nullptr ),
	OnLMBUp ( nullptr ),
	OnMMBDown ( nullptr ),
	OnMMBUp ( nullptr ),
	OnRMBDown ( nullptr ),
	OnRMBUp ( nullptr ),
	OnDoubleClick ( nullptr ),
	OnMouseMove ( nullptr ),
	OnMouseScroll ( nullptr ),
	OnMouseOver ( nullptr ),
	OnMouseLeave ( nullptr ),
	OnKeyDown ( nullptr ),
	OnKeyUp ( nullptr ),
	handler ( nullptr )
{
	// NOTHING
}

GXUIInput::~GXUIInput ()
{
	// NOTHING
}

GXVoid GXUIInput::OnMessage ( eGXUIMessage message, const GXVoid* data )
{

	if ( message == eGXUIMessage::LMBDown )
	{
		if ( !OnLMBDown ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnLMBDown ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::LMBUp )
	{
		if ( !OnLMBUp ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnLMBUp ( handler, *this, v->GetX (), v->GetY () );

		return;
	}
	
	if ( message == eGXUIMessage::MMBDown )
	{
		if ( !OnMMBDown ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnMMBDown ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::MMBUp )
	{
		if ( !OnMMBUp ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnMMBUp ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::RMBDown )
	{
		if ( !OnRMBDown ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnRMBDown ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::RMBUp )
	{
		if ( !OnRMBUp ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnRMBUp ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::Scroll )
	{
		if ( !OnMouseScroll ) return;

		const GXVec3* v = static_cast<const GXVec3*> ( data );
		OnMouseScroll ( handler, *this, v->GetX (), v->GetY (), v->GetZ () );
		return;
	}

	if ( message == eGXUIMessage::MouseMove )
	{
		if ( !OnMouseMove ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnMouseMove ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::MouseOver )
	{
		if ( !OnMouseOver ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnMouseOver ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::MouseLeave )
	{
		if ( !OnMouseLeave ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnMouseLeave ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::DoubleClick )
	{
		if ( !OnDoubleClick ) return;

		const GXVec2* v = static_cast<const GXVec2*> ( data );
		OnDoubleClick ( handler, *this, v->GetX (), v->GetY () );
		return;
	}

	if ( message == eGXUIMessage::KeyDown )
	{
		if ( !OnKeyDown ) return;

		const GXInt* keyCode = static_cast<const GXInt*> ( data );
		OnKeyDown ( handler, *this, *keyCode );
		return;
	}

	if ( message == eGXUIMessage::KeyUp )
	{
		if ( !OnKeyUp ) return;

		const GXInt* keyCode = static_cast<const GXInt*> ( data );
		OnKeyUp ( handler, *this, *keyCode );
		return;
	}

	GXWidget::OnMessage ( message, data );
}

GXVoid GXUIInput::SetOnLeftMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback )
{
	OnLMBDown = callback;
}

GXVoid GXUIInput::SetOnLeftMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback )
{
	OnLMBUp = callback;
}

GXVoid GXUIInput::SetOnMiddleMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback )
{
	OnMMBDown = callback;
}

GXVoid GXUIInput::SetOnMiddleMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback )
{
	OnMMBUp = callback;
}

GXVoid GXUIInput::SetOnRightMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback )
{
	OnRMBDown = callback;
}

GXVoid GXUIInput::SetOnRightMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEBUTTONPROC callback )
{
	OnRMBUp = callback;
}

GXVoid GXUIInput::SetOnDoubleClickCallback ( PFNGXUIINPUTNONDOUBLECLICKPROC callback )
{
	OnDoubleClick = callback;
}

GXVoid GXUIInput::SetOnMouseMoveCallback ( PFNGXUIINPUTONMOUSEMOVEPROC callback )
{
	OnMouseMove = callback;
}

GXVoid GXUIInput::SetOnMouseScrollCallback ( PFNGXUIINPUTNSCROLLPROC callback )
{
	OnMouseScroll = callback;
}

GXVoid GXUIInput::SetOnMouseOverCallback ( PFNGXUIINPUTNMOUSEOVERPROC callback )
{
	OnMouseOver = callback;
}

GXVoid GXUIInput::SetOnMouseLeaveCallback ( PFNGXUIINPUTNMOUSELEAVEPROC callback )
{
	OnMouseLeave = callback;
}

GXVoid GXUIInput::SetOnKeyDownCallback ( PFNGXUIINPUTNONKEYPROC callback )
{
	OnKeyDown = callback;
}

GXVoid GXUIInput::SetOnKeyUpCallback ( PFNGXUIINPUTNONKEYPROC callback )
{
	OnKeyUp = callback;
}

GXVoid GXUIInput::SetHandler ( GXVoid* handlerObject )
{
	handler = handlerObject;
}
