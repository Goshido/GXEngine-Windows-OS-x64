//version 1.6

#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


#define GX_FS_BUFFER_SIZE_IN_CHARACTERS		1024


GXDirectoryInfo::GXDirectoryInfo ()
{
	absolutePath = 0;

	totalFolders = 0;
	folderNames = 0;

	totalFiles = 0;
	fileNames = 0;
	fileSizes = 0;
}

GXDirectoryInfo::~GXDirectoryInfo ()
{
	Clear ();
}

GXVoid GXDirectoryInfo::Clear ()
{
	GXWChar* p = (GXWChar*)absolutePath;
	GXSafeFree ( p );

	for ( GXUInt i = 0; i < totalFolders; i++ )
	{
		p = (GXWChar*)folderNames[ i ];
		GXSafeFree ( p );
	}
	totalFolders = 0;
	GXSafeFree ( folderNames );

	for ( GXUInt i = 0; i < totalFiles; i++ )
	{
		p = (GXWChar*)fileNames[ i ];
		GXSafeFree ( p );
	}
	totalFiles = 0;
	GXSafeFree ( fileNames );
	GXULongLong* s = (GXULongLong*)fileSizes;
	GXSafeFree ( s );
}

//-----------------------------------------------------------------------

GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUInt &size, GXBool notsilent )
{
	FILE* input;
	GXUInt fileSize;
	GXUInt readed;

	_wfopen_s ( &input, fileName, L"rb" );
	if ( input == 0 )
	{
		if ( notsilent )
		{
			GXDebugBox ( L"GXLoadFile::Error - ́E�E�� �C�E��E����" );
			GXLogW ( L"GXLoadFile::Error - ́E�E�� �C�E��E���� %s\n", fileName );
		}

		*buffer = 0;
		size = 0;

		return GX_FALSE;
	}

	fseek ( input, 0, SEEK_END );
	fileSize = (GXUInt)ftell ( input );
	rewind ( input );

	if ( fileSize == 0 )
	{
		GXDebugBox ( L"GXLoadFile::Error - ���E�E��" );
		GXLogW ( L"GXLoadFile::Error - ���E%s �E��\n", fileName );

		fclose ( input );
		*buffer = 0;
		size = 0;

		return GX_FALSE;
	}

	*buffer = (GXVoid*)malloc ( fileSize );
	readed = (GXUInt)fread ( *buffer, 1, fileSize, input );
	fclose ( input );

	if ( readed != fileSize )
	{
		GXDebugBox ( L"GXLoadFile::Error - ́E�E�� �E�H����E����" );
		GXLogW ( L"GXLoadFile::Error - ́E�E�� �E�H����E���� %s\n", fileName );

		free ( *buffer );
		*buffer = 0;
		size = 0;

		return GX_FALSE;
	}

	size = fileSize;
	return GX_TRUE;
}

GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUInt size )
{
	FILE* input;
	_wfopen_s ( &input, fileName, L"wb" );

	if ( !input )
	{
		GXDebugBox ( L"GXWriteToFile::Error - ́E�E�� ������E����" );
		GXLogW ( L"GXWriteToFile::Error - ́E�E�� ������E���� %s\n", fileName );

		return GX_FALSE;
	}

	fwrite ( buffer, size, 1, input );
	fclose ( input );

	return GX_TRUE;
}

GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName )
{
	FILE* input;

	_wfopen_s ( &input, fileName, L"rb" );

	if ( input == 0 )
	{
		return GX_FALSE;
	}
	else
	{
		fclose ( input );
		return GX_TRUE;
	}
}

GXBool GXCALL GXDoesDirectoryExist ( const GXWChar* directory )
{
	DWORD ret = GetFileAttributesW ( directory ) != INVALID_FILE_ATTRIBUTES;
	return ( ret != INVALID_FILE_ATTRIBUTES ) && ( ret & FILE_ATTRIBUTE_DIRECTORY );
}

GXBool GXCALL GXCreateDirectory ( const GXWChar* directory )
{
	return CreateDirectoryW ( directory, nullptr ) ? GX_TRUE : GX_FALSE;
}

GXBool GXCALL GXGetDirectoryInfo ( GXDirectoryInfo &directoryInfo, const GXWChar* directory )
{
	if ( !directory ) return GX_FALSE;

	GXUInt size = ( GXWcslen ( directory ) + 3 ) * sizeof ( GXWChar );	// \, * and \0 symbols
	GXWChar* listedDirectory = (GXWChar*)malloc ( size );
	wsprintfW ( listedDirectory, L"%s\\*", directory );

	WIN32_FIND_DATAW info;
	HANDLE handleFind;
	handleFind = FindFirstFileW ( listedDirectory, &info );

	free ( listedDirectory );

	if ( handleFind == INVALID_HANDLE_VALUE ) return GX_FALSE;

	GXDynamicArray folderNames ( sizeof ( GXWChar* ) );
	GXDynamicArray fileNames ( sizeof ( GXWChar* ) );
	GXDynamicArray fileSizes ( sizeof ( GXULongLong ) );

	do
	{
		if ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			GXWChar* folderName;
			GXWcsclone ( &folderName, info.cFileName );
			folderNames.SetValue ( folderNames.GetLength (), &folderName );
		}
		else
		{
			GXWChar* fileName;
			GXWcsclone ( &fileName, info.cFileName );
			fileNames.SetValue ( fileNames.GetLength (), &fileName );

			GXULongLong fileSize = (GXULongLong)info.nFileSizeLow + ( (GXULongLong)info.nFileSizeHigh << 32 );
			fileSizes.SetValue ( fileSizes.GetLength (), &fileSize );
		}
	}
	while ( FindNextFileW ( handleFind, &info ) != 0 );

	if ( GetLastError () != ERROR_NO_MORE_FILES )
	{
		FindClose ( handleFind );

		GXWChar** files = (GXWChar**)fileNames.GetData ();
		GXUInt total = fileNames.GetLength ();
		for ( GXUInt i = 0; i < total; i++ )
			free ( files[ i ] );

		GXWChar** folders = (GXWChar**)folderNames.GetData ();
		total = folderNames.GetLength ();
		for ( GXUInt i = 0; i < total; i++ )
			free ( folders[ i ] );

		return GX_FALSE;
	}

	FindClose ( handleFind );

	directoryInfo.Clear ();

	GXWChar* absolutePath = (GXWChar*)malloc ( GX_FS_BUFFER_SIZE_IN_CHARACTERS * sizeof ( GXWChar) );
	GetFullPathNameW ( directory, GX_FS_BUFFER_SIZE_IN_CHARACTERS, absolutePath, 0 );
	directoryInfo.absolutePath = absolutePath;

	directoryInfo.totalFolders = folderNames.GetLength ();
	if ( directoryInfo.totalFolders > 0 )
	{
		GXUInt size = directoryInfo.totalFolders * sizeof ( GXWChar* );
		GXWChar** f = (GXWChar**)malloc ( size );
		memcpy ( f, folderNames.GetData (), size );
		directoryInfo.folderNames = (const GXWChar**)f;
	}
	else
	{
		directoryInfo.folderNames = nullptr;
	}

	directoryInfo.totalFiles = fileNames.GetLength ();
	if ( directoryInfo.totalFiles > 0 )
	{
		GXUInt size = directoryInfo.totalFiles * sizeof ( GXWChar* );
		GXWChar** f = (GXWChar**)malloc ( size );
		memcpy ( f, fileNames.GetData (), size );
		directoryInfo.fileNames = (const GXWChar**)f;

		size = directoryInfo.totalFiles * sizeof ( GXULongLong );
		GXULongLong* s = (GXULongLong*)malloc ( size );
		memcpy ( s, fileSizes.GetData (), size );
		directoryInfo.fileSizes = (const GXULongLong*)s;
	}
	else
	{
		directoryInfo.fileNames = nullptr;
		directoryInfo.fileSizes = nullptr;
	}

	return GX_TRUE;
}

GXVoid GXCALL GXGetFileDirectoryPath ( GXWChar** path, const GXWChar* absoluteFileName )
{
	if ( !absoluteFileName )
	{
		*path = nullptr;
		return;
	}

	GXInt symbols = (GXInt)GXWcslen ( absoluteFileName );
	if ( symbols == 0 )
	{
		*path = nullptr;
		return;
	}

	GXInt i = symbols;
	for ( ; i > 0; i-- )
	{
		if ( absoluteFileName[ i ] == L'\\' || absoluteFileName[ i ] == L'/' )
			break;
	}

	if ( i < 0 )
	{
		*path = nullptr;
		return;
	}


	GXUInt size = ( i + 1 ) * sizeof ( GXWChar );
	*path = (GXWChar*)malloc ( size );
	memcpy ( *path, absoluteFileName, size - sizeof ( GXWChar ) );

	( *path )[ i ] = 0;
}

GXVoid GXCALL GXGetBaseFileName ( GXWChar** fileName, const GXWChar* absoluteFileName )
{
	if ( !absoluteFileName )
	{
		*fileName = nullptr;
		return;
	}

	GXInt symbols = (GXInt)GXWcslen ( absoluteFileName );
	if ( symbols == 0 )
	{
		*fileName = nullptr;
		return;
	}

	GXInt i = symbols;
	for ( ; i > 0; i-- )
	{
		if ( absoluteFileName[ i ] == L'.' )
			break;
	}

	if ( i <= 0 )
	{
		*fileName = nullptr;
		return;
	}

	i--;
	GXInt end = i;

	for ( ; i >= 0; i-- )
	{
		if ( absoluteFileName[ i ] == L'\\' || absoluteFileName[ i ] == L'/' )
			break;
	}

	if ( i < 0 )
		i = 0;

	GXInt start = i;
	GXInt baseFileNameSymbols = end - start + 1;
	GXUInt size = ( baseFileNameSymbols + 1 ) * sizeof ( GXWChar );
	*fileName = (GXWChar*)malloc ( size );
	memcpy ( *fileName, absoluteFileName + start, size - sizeof ( GXWChar ) );

	( *fileName )[ baseFileNameSymbols ] = 0;
}

//------------------------------------------------------------------------------------------------

GXWriteStream::GXWriteStream ( const GXWChar* fileName )
{
	_wfopen_s ( &input, fileName, L"wb" );

	if ( !input )
		GXLogW ( L"GXWriteToFile::Error - Can't create file %s", fileName );
}

GXWriteStream::~GXWriteStream ()
{
	Close ();
}

GXVoid GXWriteStream::Write ( const GXVoid* data, GXUInt size )
{
	if ( !input ) return;

	fwrite ( data, size, 1, input );
}

GXVoid GXWriteStream::Close ()
{
	if ( input )
	{
		fclose ( input );
		input = 0;
	}
}
