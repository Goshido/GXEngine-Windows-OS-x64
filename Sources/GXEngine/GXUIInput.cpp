// version 1.2

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

GXVoid GXUIInput::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			if ( !OnLMBDown ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnLMBDown ( handler, *this, v->GetX (), v->GetY () );
		}
		break;

		case GX_MSG_LMBUP:
		{
			if ( !OnLMBUp ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnLMBUp ( handler, *this, v->GetX (), v->GetY () );
		}
		break;
	
		case GX_MSG_MMBDOWN:
		{
			if ( !OnMMBDown ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnMMBDown ( handler, *this, v->GetX (), v->GetY () );
		}
		break;

		case GX_MSG_MMBUP:
		{
			if ( !OnMMBUp ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnMMBUp ( handler, *this, v->GetX (), v->GetY () );
		}
		break;

		case GX_MSG_RMBDOWN:
		{
			if ( !OnRMBDown ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnRMBDown ( handler, *this, v->GetX (), v->GetY () );
		}
		break;

		case GX_MSG_RMBUP:
		{
			if ( !OnRMBUp ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnRMBUp ( handler, *this, v->GetX (), v->GetY () );
		}
		break;

		case GX_MSG_SCROLL:
		{
			if ( !OnMouseScroll ) break;

			const GXVec3* v = static_cast<const GXVec3*> ( data );
			OnMouseScroll ( handler, *this, v->GetX (), v->GetY (), v->GetZ () );
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			if ( !OnMouseMove ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnMouseMove ( handler, *this, v->GetX (), v->GetY () );
		}

		case GX_MSG_MOUSE_OVER:
		{
			if ( !OnMouseOver ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnMouseOver ( handler, *this, v->GetX (), v->GetY () );
		}

		case GX_MSG_MOUSE_LEAVE:
		{
			if ( !OnMouseLeave ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnMouseLeave ( handler, *this, v->GetX (), v->GetY () );
		}

		case GX_MSG_DOUBLE_CLICK:
		{
			if ( !OnDoubleClick ) break;

			const GXVec2* v = static_cast<const GXVec2*> ( data );
			OnDoubleClick ( handler, *this, v->GetX (), v->GetY () );
		}
		break;

		case GX_MSG_KEY_DOWN:
		{
			if ( !OnKeyDown ) break;

			const GXInt* keyCode = static_cast<const GXInt*> ( data );
			OnKeyDown ( handler, *this, *keyCode );
		}
		break;

		case GX_MSG_KEY_UP:
		{
			if ( !OnKeyUp ) break;

			const GXInt* keyCode = static_cast<const GXInt*> ( data );
			OnKeyUp ( handler, *this, *keyCode );
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
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
