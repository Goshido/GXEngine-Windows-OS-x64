//version 1.7

#ifndef GX_FILE_SYSTEM
#define GX_FILE_SYSTEM


#include "GXTypes.h"
#include <iostream>


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

GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUBigInt &size, GXBool notsilent );
GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUInt size );
GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName );
GXVoid GXCALL GXGetCurrentDirectory ( GXWChar** currentDirectory );
GXBool GXCALL GXDoesDirectoryExist ( const GXWChar* directory );
GXBool GXCALL GXCreateDirectory ( const GXWChar* directory );
GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, const GXWChar* directory );
GXVoid GXCALL GXGetFileDirectoryPath ( GXWChar** path, const GXWChar* fileName );
GXVoid GXCALL GXGetBaseFileName ( GXWChar** baseFileName, const GXWChar* fileName );
GXVoid GXCALL GXGetFileExtension ( GXWChar** extension, const GXWChar* fileName );

class GXWriteStream
{
	private:
		FILE* input;

	public:
		GXWriteStream ( const GXWChar* fileName );
		~GXWriteStream ();

		GXVoid Write ( const GXVoid* data, GXUInt size );
		GXVoid Close ();
};


#endif //GX_FILE_SYSTEM
