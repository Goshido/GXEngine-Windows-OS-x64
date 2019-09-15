// version 1.11

#include <GXEngine/GXWidget.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXLogger.h>


#define GX_WIDGET_RENDERER_RESIZE_EPSILON       0.25f

//---------------------------------------------------------------------------------------------------------------------

GXUIWidgetMessageHandlerNode::GXUIWidgetMessageHandlerNode ()
{
    // NOTHING
}

GXUIWidgetMessageHandlerNode::GXUIWidgetMessageHandlerNode ( eGXUIMessage message ):
    _message ( message )
{
    // NOTHING
}

GXUIWidgetMessageHandlerNode::~GXUIWidgetMessageHandlerNode ()
{
    // NOTHING
}

eGXCompareResult GXCALL GXUIWidgetMessageHandlerNode::Compare ( const GXAVLTreeNode& a, const GXAVLTreeNode& b )
{
    const GXUIWidgetMessageHandlerNode& aNode = static_cast<const GXUIWidgetMessageHandlerNode&> ( a );
    const GXUIWidgetMessageHandlerNode& bNode = static_cast<const GXUIWidgetMessageHandlerNode&> ( b );

    if ( aNode._message < bNode._message )
        return eGXCompareResult::Less;

    return aNode._message > bNode._message ? eGXCompareResult::Greater : eGXCompareResult::Equal;
}

//---------------------------------------------------------------------------------------------------------------------

GXWidget::GXWidget ( GXWidget* parent, GXBool isNeedRegister )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXWidget" )
    _isRegistered ( isNeedRegister ),
    _next ( nullptr ),
    _previous ( nullptr ),
    _parent ( parent ),
    _childs ( nullptr ),
    _isVisible ( GX_TRUE ),
    _isDraggable ( GX_FALSE ),
    _renderer ( nullptr ),
    _messageHandlerTree ( &GXUIWidgetMessageHandlerNode::Compare, GX_FALSE )
{
    GXAABB defaultBoundsLocal;

    defaultBoundsLocal.AddVertex ( -1.0f, -1.0f, -1.0f );
    defaultBoundsLocal.AddVertex ( 1.0f, 1.0f, 1.0f );
    UpdateBoundsWorld ( defaultBoundsLocal );

    if ( !_isRegistered ) return;

    gx_ui_SmartLock->AcquireExclusive ();

    if ( _parent )
        _parent->AddChild ( this );
    else
        GXTouchSurface::GetInstance ().RegisterWidget ( this );

    gx_ui_SmartLock->ReleaseExclusive ();
}

GXWidget::~GXWidget ()
{
    if ( !_isRegistered ) return;

    gx_ui_SmartLock->AcquireExclusive ();

    if ( _parent )
        _parent->RemoveChild ( this );
    else
        GXTouchSurface::GetInstance ().UnRegisterWidget ( this );

    gx_ui_SmartLock->ReleaseExclusive ();
}

GXVoid GXWidget::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::LMBDown )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;
            
        dest->OnMessage ( eGXUIMessage::LMBDown, data );
        return;
    }

    if ( message == eGXUIMessage::LMBUp )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::LMBUp, data );
        return;
    }

    if ( message == eGXUIMessage::MMBDown )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::MMBDown, data );
        return;
    }

    if ( message == eGXUIMessage::MMBUp )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::MMBUp, data );
        return;
    }

    if ( message == eGXUIMessage::RMBDown )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::RMBDown, data );
        return;
    }

    if ( message == eGXUIMessage::RMBUp )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::RMBUp, data );
        return;
    }

    if ( message == eGXUIMessage::Scroll )
    {
        const GXVec3* scrollData = static_cast<const GXVec3*> ( data );
        GXVec2 mousePosition ( scrollData->GetX (), scrollData->GetY () );
        GXWidget* dest = FindWidget ( mousePosition );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::Scroll, data );
        return;
    }

    if ( message == eGXUIMessage::MouseMove )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::MouseMove, data );
        return;
    }

    if ( message == eGXUIMessage::MouseOver )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::MouseOver, data );
        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXWidget* dest = FindWidget ( *pos );

        if ( dest == this ) return;

        dest->OnMessage ( eGXUIMessage::MouseLeave, data );
        return;
    }

    if ( message == eGXUIMessage::Resize )
    {
        const GXAABB* newBoundsLocal = static_cast<const GXAABB*> ( data );
        UpdateBoundsWorld ( *newBoundsLocal );

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::Show )
    {
        _isVisible = GX_TRUE;
        return;
    }

    if ( message == eGXUIMessage::Hide )
    {
        _isVisible = GX_FALSE;
        return;
    }

    if ( message == eGXUIMessage::Foreground )
    {
        if ( !_parent )
        {
            GXTouchSurface::GetInstance ().MoveWidgetToForeground ( this );
            return;
        }

        GXWidgetIterator iterator;
        GXWidget* p = iterator.Init ( this );

        while ( p->_parent )
            p = iterator.GetParent ();

        p->OnMessage ( eGXUIMessage::Foreground, data );
        return;
    }

    if ( message == eGXUIMessage::Redraw )
    {
        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }
}

GXVoid GXWidget::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    GXAABB newBounds;
    newBounds.AddVertex ( bottomLeftX, bottomLeftY, -1.0f );
    newBounds.AddVertex ( bottomLeftX + width, bottomLeftY + height, 1.0f );

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Resize, &newBounds, sizeof ( GXAABB ) );
}

const GXAABB& GXWidget::GetBoundsWorld () const
{
    return _boundsWorld;
}

const GXAABB& GXWidget::GetBoundsLocal () const
{
    return _boundsLocal;
}

GXVoid GXWidget::Show ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Show, nullptr, 0u );
}

GXVoid GXWidget::Refresh ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Redraw, nullptr, 0u );
}

GXVoid GXWidget::Hide ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Hide, nullptr, 0u );
}

GXVoid GXWidget::ToForeground ()
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::Foreground, nullptr, 0u );
}

GXBool GXWidget::IsVisible () const
{
    return _isVisible;
}

GXBool GXWidget::IsDraggable () const
{
    return _isDraggable;
}

GXVoid GXWidget::SetRenderer ( GXWidgetRenderer* renderer )
{
    _renderer = renderer;
}

GXWidgetRenderer* GXWidget::GetRenderer () const
{
    return _renderer;
}

GXWidget* GXWidget::FindWidget ( const GXVec2 &position )
{
    gx_ui_SmartLock->AcquireShared ();

    GXWidgetIterator iterator;
    GXWidget* p = iterator.Init ( _childs );

    while ( p )
    {
        if ( p->IsVisible () && p->GetBoundsWorld ().IsOverlaped ( position.GetX (), position.GetY (), 0.0f ) )
        {
            gx_ui_SmartLock->ReleaseShared ();
            return p;
        }

        p = iterator.GetNext ();
    }

    gx_ui_SmartLock->ReleaseShared ();

    return this;
}

GXVoid GXWidget::OnDraw ()
{
    if ( !_isVisible ) return;

    if ( _renderer )
        _renderer->OnDraw ();

    GXWidgetIterator iterator;
    GXWidget* p = iterator.Init ( _childs );

    while ( p )
    {
        p->OnDraw ();
        p = iterator.GetNext ();
    }
}

GXVoid GXWidget::UpdateBoundsWorld ( const GXAABB &boundsLocal )
{
    memcpy ( &_boundsLocal, &boundsLocal, sizeof ( GXAABB ) );

    if ( _parent )
    {
        const GXAABB& originWorld = _parent->GetBoundsWorld ();
        _boundsWorld.Empty ();
        _boundsWorld.AddVertex ( originWorld._min.GetX () + _boundsLocal._min.GetX (), originWorld._min.GetY () + _boundsLocal._min.GetY (), originWorld._min.GetZ () );
        _boundsWorld.AddVertex ( originWorld._min.GetX () + _boundsLocal._max.GetX (), originWorld._min.GetY () + _boundsLocal._max.GetY (), originWorld._max.GetZ () );

        return;
    }

    memcpy ( &_boundsWorld, &_boundsLocal, sizeof ( GXAABB ) );
}

GXVoid GXWidget::AddChild ( GXWidget* child )
{
    if ( DoesChildExist ( child ) ) return;

    child->_next = _childs;
    child->_previous = nullptr;

    if ( _childs )
        _childs->_previous = child;

    _childs = child;
}

GXVoid GXWidget::RemoveChild ( GXWidget* child )
{
    if ( !DoesChildExist ( child ) ) return;

    if ( child->_next ) child->_next->_previous = child->_previous;

    if ( child->_previous )
        child->_previous->_next = child->_next;
    else
        _childs = child->_next;
}

GXBool GXWidget::DoesChildExist ( GXWidget* child ) const
{
    for ( GXWidget* p = _childs; p; p = p->_next )
    {
        if ( p != child ) continue;

        return GX_TRUE;
    }

    return GX_FALSE;
}

//---------------------------------------------------------------------------------------------------------------------

GXWidgetIterator::GXWidgetIterator ():
    widget ( nullptr )
{
    // NOTHING
}

GXWidgetIterator::~GXWidgetIterator ()
{
    // NOTHING
}

GXWidget* GXWidgetIterator::Init ( GXWidget* startWidget )
{
    widget = startWidget;
    return widget;
}

GXWidget* GXWidgetIterator::GetNext ()
{
    if ( !widget ) return nullptr;

    widget = widget->_next;
    return widget;
}

GXWidget* GXWidgetIterator::GetPrevious ()
{
    if ( !widget ) return nullptr;

    widget = widget->_previous;
    return widget;
}

GXWidget* GXWidgetIterator::GetParent ()
{
    if ( !widget ) return nullptr;

    widget = widget->_parent;
    return widget;
}

GXWidget* GXWidgetIterator::GetChilds ()
{
    if ( !widget ) return nullptr;

    widget = widget->_childs;
    return widget;
}

//--------------------------------------------------------------------------

GXWidgetRenderer::GXWidgetRenderer ( GXWidget* widgetObject )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXWidgetRenderer" )
    widget ( widgetObject )
{
    // NOTHING
}

GXWidgetRenderer::~GXWidgetRenderer ()
{
    // NOTHING
}

GXVoid GXWidgetRenderer::OnUpdate ()
{
    if ( IsResized () )
    {
        const GXAABB& boundsWorld = widget->GetBoundsWorld ();

        GXUShort width = static_cast<GXUShort> ( boundsWorld.GetWidth () );
        GXUShort height = static_cast<GXUShort> ( boundsWorld.GetHeight () );

        GXVec3 center;
        boundsWorld.GetCenter ( center );
        GXRenderer& renderer = GXRenderer::GetInstance ();
        center._data[ 0u ] -= 0.5f * renderer.GetWidth ();
        center._data[ 1u ] -= 0.5f * renderer.GetHeight ();

        OnResized ( center.GetX (), center.GetY (), width, height );
        OnRefresh ();

        return;
    }

    if ( !IsMoved () )
    {
        OnRefresh ();
        return;
    }

    GXVec3 center;
    widget->GetBoundsWorld ().GetCenter ( center );
    GXRenderer& renderer = GXRenderer::GetInstance ();
    center._data[ 0u ] -= 0.5f * renderer.GetWidth ();
    center._data[ 1u ] -= 0.5f * renderer.GetHeight ();

    OnMoved ( center.GetX (), center.GetY () );
}

GXVoid GXWidgetRenderer::OnRefresh ()
{
    // NOTHING
}

GXVoid GXWidgetRenderer::OnDraw ()
{
    // NOTHING
}

GXVoid GXWidgetRenderer::OnResized ( GXFloat /*x*/, GXFloat /*y*/, GXUShort /*width*/, GXUShort /*height*/ )
{
    // NOTHING
}

GXVoid GXWidgetRenderer::OnMoved ( GXFloat /*x*/, GXFloat /*y*/ )
{
    // NOTHING
}

GXBool GXWidgetRenderer::IsResized ()
{
    if ( !widget ) return GX_FALSE;

    const GXAABB& bounds = widget->GetBoundsWorld ();

    if ( fabs ( oldBounds.GetWidth () - bounds.GetWidth () ) >= GX_WIDGET_RENDERER_RESIZE_EPSILON )
    {
        oldBounds = bounds;
        return GX_TRUE;
    }

    if ( fabs ( oldBounds.GetHeight () - bounds.GetHeight () ) < GX_WIDGET_RENDERER_RESIZE_EPSILON ) return GX_FALSE;

    oldBounds = bounds;
    return GX_TRUE;
}

GXBool GXWidgetRenderer::IsMoved ()
{
    if ( !widget ) return GX_FALSE;

    const GXAABB& bounds = widget->GetBoundsWorld ();

    if ( fabs ( bounds._min.GetX () - oldBounds._min.GetX () ) >= GX_WIDGET_RENDERER_RESIZE_EPSILON )
    {
        oldBounds = bounds;
        return GX_TRUE;
    }

    if ( fabs ( bounds._min.GetY () - oldBounds._min.GetY () ) < GX_WIDGET_RENDERER_RESIZE_EPSILON ) return GX_FALSE;

    oldBounds = bounds;
    return GX_TRUE;
}
