// version 1.5

#include <GXCommon/GXThread.h>


eGXThreadState GXAbstractThread::GetState () const
{
    return _state;
}

GXAbstractThread::GXAbstractThread ( GXThreadFunction procedure, GXVoid* argument )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXAbstractThread" )
    _procedure ( procedure ),
    _argument ( argument )
{
    _state = eGXThreadState::Waiting;
}

GXAbstractThread::~GXAbstractThread ()
{
    // NOTHING
}
