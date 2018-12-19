// version 1.4

#include <GXCommon/GXThread.h>


eGXThreadState GXAbstractThread::GetState () const
{
    return state;
}

GXAbstractThread::GXAbstractThread ( PFNGXTHREADPROC procedure, GXVoid* argument )
{
    Procedure = procedure;
    this->argument = argument;
    state = eGXThreadState::Waiting;
}

GXAbstractThread::~GXAbstractThread ()
{
    // NOTHING
}
