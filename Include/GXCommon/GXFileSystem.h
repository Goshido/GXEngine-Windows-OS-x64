//version 1.6

#ifndef GX_FILE_SYSTEM
#define GX_FILE_SYSTEM


#include "GXCommon.h"
#include <iostream>


struct GXDirectoryInfo
{
	const GXWChar*		absolutePath;

	GXUInt				totalFolders;
	const GXWChar**		folderNames;

	GXUInt				totalFiles;
	const GXWChar**		fileNames;
	const GXULongLong*	fileSizes;

	GXDirectoryInfo ();
	~GXDirectoryInfo ();

	GXVoid Clear ();
};

GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUInt &size, GXBool notsilent );
GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUInt size );
GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName );
GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, const GXWChar* directory );
GXVoid GXCALL GXGetFilePath ( GXWChar** path, const GXWChar fullFileName );

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
