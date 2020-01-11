// version 1.5

#ifndef GX_THREAD
#define GX_THREAD


#include "GXMemory.h"


class GXThread;
typedef GXUPointer ( GXTHREADCALL* GXThreadFunction ) ( GXVoid* argument, GXThread &thread );


enum class eGXThreadState : GXUByte
{
    Waiting,
    Started
};

// Note this is GXMemoryInspector class. Please use GX_BIND_MEMORY_INSPECTOR_CLASS_NAME with new.
class GXAbstractThread : public GXMemoryInspector
{
    protected:
        GXThreadFunction    _procedure;
        GXVoid*             _argument;
        eGXThreadState      _state;

    public:
        eGXThreadState GetState () const;

        virtual GXVoid Start () = 0;
        virtual GXVoid Switch () = 0;
        virtual GXVoid Sleep ( GXUInt milliseconds ) = 0;
        virtual GXVoid Join () = 0;

    protected:
        explicit GXAbstractThread ( GXThreadFunction procedure, GXVoid* argument );
        virtual ~GXAbstractThread ();

    private:
        GXAbstractThread () = delete;
        GXAbstractThread ( const GXAbstractThread &other ) = delete;
        GXAbstractThread& operator = ( const GXAbstractThread &other ) = delete;
};


#ifdef __GNUC__

// TODO implement this
#include "Posix/GXThread.h"

#else

#include "Windows/GXThread.h"

#endif // __GNUC__


#endif // GX_THREAD
