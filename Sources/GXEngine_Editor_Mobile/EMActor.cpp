#include <GXEngine_Editor_Mobile/EMActor.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


EMActor* em_Actors = nullptr;

EMActor::EMActor ( const GXWChar* name, eEMActorType type, const GXTransform &transform ):
    _next ( em_Actors ),
    _previous ( nullptr ),
    _type ( type ),
    _transform ( transform ),
    _isVisible ( GX_TRUE )
{
    GXWcsclone ( &this->_name, name );

    if ( em_Actors )
        em_Actors->_previous = this;

    em_Actors = this;
}

EMActor::~EMActor ()
{
    if ( _next ) _next->_previous = _previous;

    if ( _previous ) 
        _previous->_next = _next;
    else
        em_Actors = _next;

    free ( _name );
}

GXVoid EMActor::OnDrawCommonPass ( GXFloat /*deltaTime*/ )
{
    // NOTHING
}

GXVoid EMActor::OnDrawHudColorPass ()
{
    // NOTHING
}

GXVoid EMActor::OnDrawHudMaskPass ()
{
    // NOTHING
}

GXVoid EMActor::OnUpdate ( GXFloat /*deltaTime*/ )
{
    // NOTHING
}

GXVoid EMActor::OnSave ( GXUByte** /*data*/ )
{
    // NOTHING
}

GXVoid EMActor::OnLoad ( const GXUByte* /*data*/ )
{
    // NOTHING
}

GXUPointer EMActor::OnRequeredSaveSize () const
{
    // NOTHING
    return 0u;
}

GXVoid EMActor::OnTransformChanged ()
{
    // NOTHING
}

const GXWChar* EMActor::GetName ()
{
    return _name;
}

eEMActorType EMActor::GetType ()
{
    return _type;
}

const GXTransform& EMActor::GetTransform ()
{
    return _transform;
}

GXVoid EMActor::SetTransform ( const GXTransform &newTransform )
{
    _transform = newTransform;
    OnTransformChanged ();
}

GXVoid EMActor::Show ()
{
    _isVisible = GX_TRUE;
}

GXVoid EMActor::Hide ()
{
    _isVisible = GX_FALSE;
}
