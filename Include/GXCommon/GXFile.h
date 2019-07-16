// version 1.1

#ifndef GX_FILE
#define GX_FILE


#include "GXStrings.h"


enum class eGXFileContentOwner : GXUByte
{
    GXFile,
    User
};

class GXFile final : public GXMemoryInspector
{
    protected:
        GXString                _path;
        GXUByte*                _content;
        GXUPointer              _size;
        eGXFileContentOwner     _owner;

    public:
        explicit GXFile ( GXString fileName );
        ~GXFile () override;

        // Method returns GX_TRUE if file was successfuly loaded.
        // Note warning message box will appear if file content
        // could be not readed and notSilent equals GX_TRUE.
        // Content will be freed on GXFile destructor or Close method
        // if contentOwner equals eGXFileContentOwner::GXFile. Otherwise
        // user code MUST free content itself by GXMemoryInspector::Free API.
        // Freeing content by ::free IS NOT allowed!
        GXBool LoadContent ( GXUByte*& fileContent, GXUPointer &fileSize, eGXFileContentOwner contentOwner, GXBool notSilent );

        GXString GetPath () const;
        GXVoid Close ();

    private:
        GXFile () = delete;
        GXFile ( const GXFile &other ) = delete;
        GXFile& operator = ( const GXFile &other ) = delete;
};


#endif // GX_FILE
