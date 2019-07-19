// version 1.0

#ifndef GX_WRITE_FILE_STREAM
#define GX_WRITE_FILE_STREAM


#include "GXStrings.h"


class GXAbstractWriteFileStream
{
    protected:
        const GXString      _fileName;

    public:
        explicit GXAbstractWriteFileStream ( GXString fileName );
        virtual ~GXAbstractWriteFileStream ();

        virtual GXVoid Write ( const GXVoid* data, GXUPointer size ) = 0;
        virtual GXVoid Flush () = 0;
        virtual GXVoid Close () = 0;

    private:
        GXAbstractWriteFileStream () = delete;
        GXAbstractWriteFileStream ( const GXAbstractWriteFileStream &other ) = delete;
        GXAbstractWriteFileStream& operator = ( const GXAbstractWriteFileStream &other ) = delete;
};


#ifdef __GNUC__

#include "Posix/GXWriteFileStream.h"

#else

#include "Windows/GXWriteFileStream.h"

#endif // __GNU__


#endif // GX_WRITE_FILE_STREAM
