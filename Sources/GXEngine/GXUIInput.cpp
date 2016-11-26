//version 1.0

#include <GXEngine/GXUIInput.h>
#include <GXEngine/GXUIMessage.h>


GXUIInput::GXUIInput ( GXWidget* parent, GXBool isNeedRegister ):
GXWidget ( parent, isNeedRegister )
{
	onLMBDownCallback = nullptr;
	onLMBUpCallback = nullptr;
	onMMBDownCallback = nullptr;
	onMMBUpCallback = nullptr;
	onRMBDownCallback = nullptr;
	onRMBUpCallback = nullptr;

	onDoubleClickCallback = nullptr;
	onMouseMoveCallback = nullptr;
	onMouseScrollCallback = nullptr;

	handler = nullptr;
}

GXUIInput::~GXUIInput ()
{
	//NOTHING
}

GXVoid GXUIInput::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onLMBDownCallback )
				onLMBDownCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onLMBUpCallback )
				onLMBUpCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;
	
		case GX_MSG_MMBDOWN:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onMMBDownCallback )
				onMMBDownCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MMBUP:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onMMBUpCallback )
				onMMBUpCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_RMBDOWN:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onRMBDownCallback )
				onRMBDownCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_RMBUP:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onRMBUpCallback )
				onRMBUpCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SCROLL:
		{
			const GXVec3* v = (const GXVec3*)data;
			if ( onMouseScrollCallback )
				onMouseScrollCallback ( handler, this, v->x, v->y, v->z );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onMouseMoveCallback )
				onMouseMoveCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}

		case GX_MSG_DOUBLE_CLICK:
		{
			const GXVec2* v = (const GXVec2*)data;
			if ( onDoubleClickCallback )
				onDoubleClickCallback ( handler, this, v->x, v->y );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIInput::SetOnLeftMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onLMBDownCallback = callback;
}

GXVoid GXUIInput::SetOnLeftMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onLMBUpCallback = callback;
}

GXVoid GXUIInput::SetOnMiddleMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onMMBDownCallback = callback;
}

GXVoid GXUIInput::SetOnMiddleMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onMMBUpCallback = callback;
}

GXVoid GXUIInput::SetOnRightMouseButtonDownCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onRMBDownCallback = callback;
}

GXVoid GXUIInput::SetOnRightMouseButtonUpCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onRMBUpCallback = callback;
}

GXVoid GXUIInput::SetOnDoubleClickCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onDoubleClickCallback = callback;
}

GXVoid GXUIInput::SetOnMouseMoveCallback ( PFNGXUIINPUTONMOUSEPOSITIONPROC callback )
{
	onMouseMoveCallback = callback;
}

GXVoid GXUIInput::SetOnMouseScrollCallback ( PFNGXUIINPUTONMOUSESCROLLPROC callback )
{
	onMouseScrollCallback = callback;
}

GXVoid GXUIInput::SetHandler ( GXVoid* handler )
{
	this->handler = handler;
}
