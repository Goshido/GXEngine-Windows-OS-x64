// version 1.15

#ifndef GX_FILE_SYSTEM
#define GX_FILE_SYSTEM


#include "GXStrings.h"

GX_DISABLE_COMMON_WARNINGS

#include <iostream>

GX_RESTORE_WARNING_STATE


struct GXDirectoryInfo final
{
    const GXWChar*      _absolutePath;

    GXUInt              _totalFolders;
    const GXWChar**     _folderNames;

    GXUInt              _totalFiles;
    const GXWChar**     _fileNames;
    const GXUBigInt*    _fileSizes;

    GXDirectoryInfo ();
    ~GXDirectoryInfo ();

    GXVoid Clear ();

    GXDirectoryInfo ( const GXDirectoryInfo &other ) = delete;
    GXDirectoryInfo& operator = ( const GXDirectoryInfo &other ) = delete;
};

// Directory can be created recursively.
GXBool GXCALL GXCreateDirectory ( GXString directory );
GXString GXCALL GXGetCurrentDirectory ();
GXBool GXCALL GXDoesDirectoryExist ( GXString directory );
GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, GXString directory );

GXBool GXCALL GXDoesFileExist ( GXString fileName );
GXBool GXCALL GXWriteToFile ( GXString fileName, const GXVoid* buffer, GXUPointer size );
GXString GXCALL GXGetBaseFileName ( GXString fileName );
GXString GXCALL GXGetFileExtension ( GXString fileName );

GXString GXCALL GXGetFileDirectoryPath ( GXString fileName );

GXString GXCALL GXWordExp ( GXString expression );


#endif // GX_FILE_SYSTEM
