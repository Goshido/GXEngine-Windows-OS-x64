// version 1.1

#include <GXCommon/Windows/GXWriteFileStream.h>
#include <GXCommon/GXLogger.h>


GXWriteFileStream::GXWriteFileStream ( GXString fileName ):
    GXAbstractWriteFileStream ( fileName )
{
    _wfopen_s ( &_file, _fileName, L"wb" );

    if ( _file ) return;

    GXLogW ( L"GXWriteToFile::Error - Не могу создать файл %.\n", static_cast<const GXWChar*> ( _fileName ) );
}

GXWriteFileStream::~GXWriteFileStream ()
{
    Close ();
}

GXVoid GXWriteFileStream::Write ( const GXVoid* data, GXUPointer size )
{
    if ( !_file ) return;

    fwrite ( data, size, 1u, _file );
}

GXVoid GXWriteFileStream::Flush ()
{
    fflush ( _file );
}

GXVoid GXWriteFileStream::Close ()
{
    if ( !_file ) return;

    fclose ( _file );
    _file = nullptr;
}
