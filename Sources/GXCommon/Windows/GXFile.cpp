// version 1.0

#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>


GXFile::GXFile ( GXString fileName )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXFile" )
    path ( fileName ),
    content ( nullptr ),
    size ( 0u ),
    owner ( eGXFileContentOwner::GXFile )
{
    // NOTHING
}

GXFile::~GXFile ()
{
    Close ();
}

GXBool GXFile::LoadContent ( GXUByte*& fileContent, GXUPointer &fileSize, eGXFileContentOwner contentOwner, GXBool notSilent )
{
    if ( content )
    {
        if ( contentOwner == eGXFileContentOwner::User )
        {
            fileContent = static_cast<GXUByte*> ( Malloc ( size ) );
            memcpy ( fileContent, content, size );
        }
        else
        {
            fileContent = content;
        }

        fileSize = size;
        return GX_TRUE;
    }

    FILE* file = nullptr;
    _wfopen_s ( &file, path, L"rb" );

    if ( !file )
    {
        if ( notSilent )
        {
            GXString errorMessage;
            errorMessage.Format ( "GXFile::LoadContent::Error - Не могу открыть файл %s.\n", static_cast<const GXMBChar*> ( path ) );

            GXLogA ( errorMessage );
            GXWarningBox ( errorMessage );
        }

        fileContent = nullptr;
        fileSize = 0u;

        return GX_FALSE;
    }

    fseek ( file, 0, SEEK_END );
    GXUPointer newSize = static_cast<GXUPointer> ( ftell ( file ) );
    rewind ( file );

    if ( newSize == 0u )
    {
        if ( notSilent )
        {
            GXString errorMessage;
            errorMessage.Format ( "GXFile::LoadContent::Error - Файл %s пуст.\n", static_cast<const GXMBChar*> ( path ) );

            GXLogA ( errorMessage );
            GXWarningBox ( errorMessage );
        }

        fclose ( file );

        fileContent = nullptr;
        fileSize = 0u;

        return GX_FALSE;
    }

    GXUByte* newContent = static_cast<GXUByte*> ( Malloc ( newSize ) );
    GXUPointer readed = static_cast<GXUPointer> ( fread ( newContent, 1, newSize, file ) );

    fclose ( file );
    Close ();

    if ( readed == newSize )
    {
        Close ();

        fileContent = content = newContent;
        fileSize = size = newSize;
        owner = contentOwner;

        return GX_TRUE;
    }

    if ( notSilent )
    {
        GXString errorMessage;
        errorMessage.Format ( "GXFile::LoadContent::Error - Не могу прочесть файл %s.\n", static_cast<const GXMBChar*> ( path ) );

        GXLogA ( errorMessage );
        GXWarningBox ( errorMessage );
    }

    SafeFree ( reinterpret_cast<GXVoid**> ( &newContent ) );

    fileContent = nullptr;
    fileSize = 0u;

    return GX_FALSE;
}

GXString GXFile::GetPath () const
{
    return path;
}

GXVoid GXFile::Close ()
{
    if ( !content || owner != eGXFileContentOwner::GXFile ) return;

    SafeFree ( reinterpret_cast<GXVoid**> ( &content ) );
    size = 0u;
}
