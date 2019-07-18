// version 1.14

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
};

GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUPointer &size, GXBool notsilent );
GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUPointer size );
GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName );
GXVoid GXCALL GXGetCurrentDirectory ( GXWChar** currentDirectory );
GXBool GXCALL GXDoesDirectoryExist ( const GXWChar* directory );

// Directory can be created recursively.
GXBool GXCALL GXCreateDirectory ( const GXWChar* directory );
GXBool GXCALL GXCreateDirectory ( GXString directory );

GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, const GXWChar* directory );
GXVoid GXCALL GXGetFileDirectoryPath ( GXWChar** path, const GXWChar* fileName );
GXVoid GXCALL GXGetBaseFileName ( GXWChar** baseFileName, const GXWChar* fileName );
GXVoid GXCALL GXGetFileExtension ( GXWChar** extension, const GXWChar* fileName );

GXString GXCALL GXWordExp ( GXString expression );

class GXWriteFileStream
{
    private:
        FILE*       _file;

    public:
        explicit GXWriteFileStream ( const GXWChar* fileName );
        ~GXWriteFileStream ();

        GXVoid Write ( const GXVoid* data, GXUPointer size );
        GXVoid Flush ();
        GXVoid Close ();

    private:
        GXWriteFileStream ( const GXWriteFileStream &other ) = delete;
        GXWriteFileStream& operator = ( const GXWriteFileStream &other ) = delete;
};


#endif // GX_FILE_SYSTEM
