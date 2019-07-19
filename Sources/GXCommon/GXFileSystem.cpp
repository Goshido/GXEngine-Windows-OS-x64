// version 1.15

#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


GXDirectoryInfo::GXDirectoryInfo ()
{
    _absolutePath = nullptr;

    _totalFolders = 0u;
    _folderNames = nullptr;

    _totalFiles = 0u;
    _fileNames = nullptr;
    _fileSizes = nullptr;
}

GXDirectoryInfo::~GXDirectoryInfo ()
{
    Clear ();
}

GXVoid GXDirectoryInfo::Clear ()
{
    GXWChar* p = const_cast<GXWChar*> ( _absolutePath );
    GXSafeFree ( p );
    _absolutePath = nullptr;

    for ( GXUInt i = 0; i < _totalFolders; ++i )
    {
        p = const_cast<GXWChar*> ( _folderNames[ i ] );
        GXSafeFree ( p );
    }

    _totalFolders = 0u;
    GXSafeFree ( _folderNames );

    for ( GXUInt i = 0u; i < _totalFiles; ++i )
    {
        p = (GXWChar*)_fileNames[ i ];
        GXSafeFree ( p );
    }

    _totalFiles = 0u;
    GXSafeFree ( _fileNames );

    GXUBigInt* s = const_cast<GXUBigInt*> ( _fileSizes );
    GXSafeFree ( s );
    _fileSizes = nullptr;
}
