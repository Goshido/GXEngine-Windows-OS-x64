// version 1.13

#ifndef GX_FILE_SYSTEM
#define GX_FILE_SYSTEM


#include "GXTypes.h"

GX_DISABLE_COMMON_WARNINGS

#include <iostream>

GX_RESTORE_WARNING_STATE


struct GXDirectoryInfo
{
    const GXWChar*      absolutePath;

    GXUInt              totalFolders;
    const GXWChar**     folderNames;

    GXUInt              totalFiles;
    const GXWChar**     fileNames;
    const GXUBigInt*    fileSizes;

    GXDirectoryInfo ();
    ~GXDirectoryInfo ();

    GXVoid Clear ();
};

GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUPointer &size, GXBool notsilent );
GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUPointer size );
GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName );
GXVoid GXCALL GXGetCurrentDirectory ( GXWChar** currentDirectory );
GXBool GXCALL GXDoesDirectoryExist ( const GXWChar* directory );
GXBool GXCALL GXCreateDirectory ( const GXWChar* directory );
GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, const GXWChar* directory );
GXVoid GXCALL GXGetFileDirectoryPath ( GXWChar** path, const GXWChar* fileName );
GXVoid GXCALL GXGetBaseFileName ( GXWChar** baseFileName, const GXWChar* fileName );
GXVoid GXCALL GXGetFileExtension ( GXWChar** extension, const GXWChar* fileName );

class GXWriteFileStream
{
    private:
        FILE*       file;

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
