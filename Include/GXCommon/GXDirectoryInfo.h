// version 1.0

#ifndef GX_DIRECTORY_INFO
#define GX_DIRECTORY_INFO


#include "GXStrings.h"
#include "GXMemory.h"


struct GXAbstractDirectoryInfo : public GXMemoryInspector
{
    protected:
        // GXString elements which Must be created via placement new operator.
        GXDynamicArray      _directories;

        // GXString elements which Must be created via placement new operator.
        GXDynamicArray      _files;

        // GXUBigInt elements.
        GXDynamicArray      _fileSizes;

        const GXString      _targetDirectory;

        GXString            _absolutePath;

    public:
        GXString GetAbsolutePath () const;

        GXUPointer GetDirectoryCount () const;
        GXString GetDirectoryName ( GXUPointer directoryIndex ) const;

        GXUPointer GetFileCount () const;
        GXString GetFileName ( GXUPointer fileIndex ) const;
        GXUBigInt GetFileSize ( GXUPointer fileIndex ) const;

    protected:
        // Note backend constructor MUST fill all filelds.
        explicit GXAbstractDirectoryInfo ( GXString targetDirectory );

        // Note destructor will free _directories and _files array data automatically.
        ~GXAbstractDirectoryInfo () override;

    private:
        GXAbstractDirectoryInfo () = delete;
        GXAbstractDirectoryInfo ( const GXAbstractDirectoryInfo &other ) = delete;
        GXAbstractDirectoryInfo& operator = ( const GXAbstractDirectoryInfo &other ) = delete;
};


#ifdef __GNUC__

#include "Posix/GXDirectoryInfo.h"

#else

#include "Windows/GXDirectoryInfo.h"

#endif // __GNUC__


#endif // GX_DIRECTORY_INFO
