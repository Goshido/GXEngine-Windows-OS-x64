// version 1.0

#include <GXCommon/GXDirectoryInfo.h>


GXString GXAbstractDirectoryInfo::GetAbsolutePath () const
{
    return _absolutePath;
}

GXUPointer GXAbstractDirectoryInfo::GetDirectoryCount () const
{
    return _directories.GetLength ();
}

GXString GXAbstractDirectoryInfo::GetDirectoryName ( GXUPointer directoryIndex ) const
{
    if ( directoryIndex >= _directories.GetLength () )
        return {};

    return *( static_cast<GXString*> ( _directories.GetValue ( directoryIndex ) ) );
}

GXUPointer GXAbstractDirectoryInfo::GetFileCount () const
{
    return _files.GetLength ();
}

GXString GXAbstractDirectoryInfo::GetFileName ( GXUPointer fileIndex ) const
{
    if ( fileIndex >= _files.GetLength () )
        return {};

    return *( static_cast<GXString*> ( _files.GetValue ( fileIndex ) ) );
}

GXUBigInt GXAbstractDirectoryInfo::GetFileSize ( GXUPointer fileIndex ) const
{
    if ( fileIndex >= _fileSizes.GetLength () )
        return {};

    return *( static_cast<const GXUBigInt*> ( _fileSizes.GetValue ( fileIndex ) ) );
}

GXAbstractDirectoryInfo::GXAbstractDirectoryInfo ( GXString targetDirectory )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXAbstractDirectoryInfo" )
    _directories ( sizeof ( GXString ) ),
    _files ( sizeof ( GXString ) ),
    _fileSizes ( sizeof ( GXUBigInt ) ),
    _targetDirectory ( targetDirectory )
{
    // NOTHING
}

GXAbstractDirectoryInfo::~GXAbstractDirectoryInfo ()
{
    GXUPointer count = _directories.GetLength ();

    for ( GXUPointer i = 0u; i < count; ++i )
    {
        GXString* directory = static_cast<GXString*> ( _directories.GetValue ( i ) );
        directory->Clear ();
    }

    count = _files.GetLength ();

    for ( GXUPointer i = 0u; i < count; ++i )
    {
        GXString* file = static_cast<GXString*> ( _files.GetValue ( i ) );
        file->Clear ();
    }
}
