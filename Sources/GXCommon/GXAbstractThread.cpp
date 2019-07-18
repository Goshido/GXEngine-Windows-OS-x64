// version 1.5

#include <GXCommon/GXThread.h>


eGXThreadState GXAbstractThread::GetState () const
{
    return _state;
}

GXAbstractThread::GXAbstractThread ( GXThreaFunction procedure, GXVoid* argument )
{
    _procedure = procedure;
    _argument = argument;
    _state = eGXThreadState::Waiting;
}

GXAbstractThread::~GXAbstractThread ()
{
    // NOTHING
}
