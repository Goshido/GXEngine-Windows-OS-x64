// version 1.1

#include <GXCommon/GXThread.h>


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

eGXThreadState GXAbstractThread::GetState () const
{
	return state;
}
