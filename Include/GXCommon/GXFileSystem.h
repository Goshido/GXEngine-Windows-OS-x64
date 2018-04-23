// version 1.8

#ifndef GX_FILE_SYSTEM
#define GX_FILE_SYSTEM


#include "GXTypes.h"
#include "GXDisable3rdPartyWarnings.h"
#include <iostream>
#include "GXRestoreWarnings.h"


struct GXDirectoryInfo
{
	const GXWChar*		absolutePath;

	GXUInt				totalFolders;
	const GXWChar**		folderNames;

	GXUInt				totalFiles;
	const GXWChar**		fileNames;
	const GXUBigInt*	fileSizes;

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
		FILE* file;

	public:
		GXWriteFileStream ( const GXWChar* fileName );
		~GXWriteFileStream ();

		GXVoid Write ( const GXVoid* data, GXUPointer size );
		GXVoid Flush ();
		GXVoid Close ();
};


#endif // GX_FILE_SYSTEM
