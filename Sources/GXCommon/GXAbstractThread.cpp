// version 1.5

#include <GXCommon/GXThread.h>


eGXThreadState GXAbstractThread::GetState () const
{
    return _state;
}

GXAbstractThread::GXAbstractThread ( PFNGXTHREADPROC procedure, GXVoid* argument )
{
    _procedure = procedure;
    this->_argument = argument;
    _state = eGXThreadState::Waiting;
}

GXAbstractThread::~GXAbstractThread ()
{
    // NOTHING
}
