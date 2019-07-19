// version 1.1

#ifndef GX_WRITE_FILE_STREAM_WINDOWS
#define GX_WRITE_FILE_STREAM_WINDOWS


#include <GXCommon/GXWriteFileStream.h>

GX_DISABLE_COMMON_WARNINGS

#include <iostream>

GX_RESTORE_WARNING_STATE


class GXWriteFileStream final : public GXAbstractWriteFileStream
{
    private:
        FILE*       _file;

    public:
        explicit GXWriteFileStream ( GXString fileName );
        ~GXWriteFileStream ();

        GXVoid Write ( const GXVoid* data, GXUPointer size ) override;
        GXVoid Flush () override;
        GXVoid Close () override;

    private:
        GXWriteFileStream () = delete;
        GXWriteFileStream ( const GXWriteFileStream &other ) = delete;
        GXWriteFileStream& operator = ( const GXWriteFileStream &other ) = delete;
};


#endif // GX_WRITE_FILE_STREAM_WINDOWS
