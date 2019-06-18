// version 1.10

#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXLocale.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXUPointerAtomic.h>
#include <GXCommon/GXLogger.h>


#define PRECOMPILED_SHADER_PROGRAM_DICTIONARY       L"Shaders/Precompiled shader program dictionary.bin"
#define PRECOMPILED_SHADER_PROGRAM_INFO             L"Shaders/Precompiled shader programs.info"
#define PRECOMPILED_SHADER_PROGRAM_DIRECTORY        L"Shaders/Precompiled shader programs"
#define PRECOMPILED_SHADER_PROGRAM_EXTENSION        L"spr"

#define NULL_STRING_OFFSET                          0u

#define INVALID_SHADER                              0u
#define SHADER_NOT_NEEDED                           0u
#define INVALID_SHADER_PROGRAM                      0u

//---------------------------------------------------------------------------------------------------------------------

class GXPrecompiledShaderProgramFinder;
class GXPrecompiledShaderProgramNode final : public GXAVLTreeNode
{
    friend class GXPrecompiledShaderProgramFinder;

    private:
        GLenum      _binaryFormat;
        GXWChar*    _binaryPath;
        GXWChar*    _vertexShader;
        GXWChar*    _geometryShader;
        GXWChar*    _fragmentShader;

    private:
        GXPrecompiledShaderProgramNode ();
        explicit GXPrecompiledShaderProgramNode ( const GXUTF8* vertexShader, const GXUTF8* fragmentShader, const GXUTF8* geometryShader, const GXUTF8* binaryPath, GLenum binaryFormat );
        explicit GXPrecompiledShaderProgramNode ( const GXWChar* vertexShader, const GXWChar* fragmentShader, const GXWChar* geometryShader, const GXWChar* binaryPath, GLenum binaryFormat );
        ~GXPrecompiledShaderProgramNode () override;

        static GXInt GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );
        static GXVoid GXCALL InitFinderNode ( GXPrecompiledShaderProgramNode& node, const GXWChar* vertexShader, const GXWChar* fragmentShader, const GXWChar* geometryShader );
        static GXVoid GXCALL DestroyFinderNode ( GXPrecompiledShaderProgramNode& node );

        GXPrecompiledShaderProgramNode ( const GXPrecompiledShaderProgramNode &other ) = delete;
        GXPrecompiledShaderProgramNode& operator = ( const GXPrecompiledShaderProgramNode &other ) = delete;
};

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ():
    _binaryPath ( nullptr ),
    _binaryFormat ( GL_INVALID_ENUM ),
    _vertexShader ( nullptr ),
    _geometryShader ( nullptr ),
    _fragmentShader ( nullptr )
{
    // NOTHING
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXUTF8* vertexShader, const GXUTF8* geometryShader, const GXUTF8* fragmentShader, const GXUTF8* binaryPath, GLenum binaryFormat ):
    _binaryFormat ( binaryFormat )
{
    GXToWcs ( &this->_binaryPath, binaryPath );
    GXToWcs ( &this->_vertexShader, vertexShader );

    if ( geometryShader )
        GXToWcs ( &this->_geometryShader, geometryShader );
    else
        this->_geometryShader = nullptr;

    if ( fragmentShader )
    {
        GXToWcs ( &this->_fragmentShader, fragmentShader );
        return;
    }

    this->_fragmentShader = nullptr;
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXWChar* vertexShader, const GXWChar* geometryShader, const GXWChar* fragmentShader, const GXWChar* binaryPath, GLenum binaryFormat ):
    _binaryFormat ( binaryFormat )
{
    GXWcsclone ( &this->_binaryPath, binaryPath );
    GXWcsclone ( &this->_vertexShader, vertexShader );

    if ( geometryShader )
        GXWcsclone ( &this->_geometryShader, geometryShader );
    else
        this->_geometryShader = nullptr;

    if ( fragmentShader )
    {
        GXWcsclone ( &this->_fragmentShader, fragmentShader );
        return;
    }

    this->_fragmentShader = nullptr;
}

GXPrecompiledShaderProgramNode::~GXPrecompiledShaderProgramNode ()
{
    free ( _binaryPath );
    free ( _vertexShader );
    GXSafeFree ( _geometryShader );
    GXSafeFree ( _fragmentShader );
}

GXInt GXCALL GXPrecompiledShaderProgramNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXPrecompiledShaderProgramNode& aNode = static_cast<const GXPrecompiledShaderProgramNode&> ( a );
    const GXPrecompiledShaderProgramNode& bNode = static_cast<const GXPrecompiledShaderProgramNode&> ( b );

    GXInt compare = GXWcscmp ( aNode._fragmentShader, bNode._fragmentShader );

    if ( compare != 0 )
        return compare;

    compare = GXWcscmp ( aNode._vertexShader, bNode._vertexShader );

    if ( compare != 0 )
        return compare;

    return GXWcscmp ( aNode._geometryShader, bNode._geometryShader );
}

GXVoid GXCALL GXPrecompiledShaderProgramNode::InitFinderNode ( GXPrecompiledShaderProgramNode& node, const GXWChar* vertexShader, const GXWChar* fragmentShader, const GXWChar* geometryShader )
{
    node._vertexShader = const_cast<GXWChar*> ( vertexShader );
    node._geometryShader = const_cast<GXWChar*> ( geometryShader );
    node._fragmentShader = const_cast<GXWChar*> ( fragmentShader );
    node._binaryPath = nullptr;
}

GXVoid GXCALL GXPrecompiledShaderProgramNode::DestroyFinderNode ( GXPrecompiledShaderProgramNode& node )
{
    node._vertexShader = nullptr;
    node._geometryShader = nullptr;
    node._fragmentShader = nullptr;
    node._binaryPath = nullptr;
}

//---------------------------------------------------------------------------------------------------------------------

#pragma pack ( push )
#pragma pack ( 1 )

struct GXDictionaryHeader final
{
    GXUBigInt       _counter;
    GXUInt          _totalPrecompiledPrograms;
    GXUBigInt       _chunkOffset;
};

struct GXChunk final
{
    GXUBigInt       _vertexShaderOffset;
    GXUBigInt       _geometryShaderOffset;
    GXUBigInt       _fragmentShaderOffset;
    GXUBigInt       _binaryPathOffset;
    GLenum          _binaryFormat;
};

#pragma pack ( pop )

//---------------------------------------------------------------------------------------------------------------------

class GXSaveState final
{
    public:
        GXWriteFileStream       _info;

        GXUByte*                _content;
        GXUBigInt               _chunckOffset;
        GXUBigInt               _utf8StringOffset;

    public:
        explicit GXSaveState ( const GXWChar* infoFile );
        ~GXSaveState ();

    private:
        GXSaveState () = delete;
        GXSaveState ( const GXSaveState &other ) = delete;
        GXSaveState& operator = ( const GXSaveState &other ) = delete;
};

GXSaveState::GXSaveState ( const GXWChar* infoFile ):
    _info ( infoFile )
{
    // NOTHING
}

GXSaveState::~GXSaveState ()
{
    _info.Close ();
}

//---------------------------------------------------------------------------------------------------------------------

class GXPrecompiledShaderProgramFinder final : public GXMemoryInspector, public GXAVLTree
{
    friend class GXShaderProgramEntry;

    private:
        GXUBigInt       _counter;

    private:
        GXPrecompiledShaderProgramFinder ();
        ~GXPrecompiledShaderProgramFinder () override;

        GXBool FindProgram ( const GXWChar** binaryPath, GLenum &binaryFormat, const GXWChar* vertexShader, const GXWChar* geometryShader, const GXWChar* fragmentShader ) const;
        GXVoid AddProgram ( const GXWChar* vertexShader, const GXWChar* geometryShader, const GXWChar* fragmentShader, const GXWChar* binaryPath, GLenum binaryFormat );
        GXUBigInt GetCounter ();

        static GXVoid GXCALL GetDictionarySize ( const GXAVLTreeNode &node, GXVoid* args );
        static GXVoid GXCALL SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args );

        GXPrecompiledShaderProgramFinder ( const GXPrecompiledShaderProgramFinder &other ) = delete;
        GXPrecompiledShaderProgramFinder& operator = ( const GXPrecompiledShaderProgramFinder &other ) = delete;
};

GXPrecompiledShaderProgramFinder::GXPrecompiledShaderProgramFinder ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXPrecompiledShaderProgramFinder" )
    GXAVLTree ( &GXPrecompiledShaderProgramNode::Compare, GX_TRUE )
{
    GXUByte* data;
    GXUPointer size;

    GXFile file ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY );

    if ( !file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
    {
        _counter = 0u;
        return;
    }

    GXDictionaryHeader* header = reinterpret_cast<GXDictionaryHeader*> ( data );
    GXChunk* chuncks = reinterpret_cast<GXChunk*> ( data + header->_chunkOffset );
    _counter = header->_counter;

    for ( GXUInt i = 0u; i < header->_totalPrecompiledPrograms; ++i )
    {
        const GXChunk& chunk = chuncks[ i ];

        const GXUTF8* vertexShader = chunk._vertexShaderOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk._vertexShaderOffset );
        const GXUTF8* geometryShader = chunk._geometryShaderOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk._geometryShaderOffset );
        const GXUTF8* fragmentShader = chunk._fragmentShaderOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk._fragmentShaderOffset );
        const GXUTF8* binaryPath = reinterpret_cast<const GXUTF8*> ( data + chunk._binaryPathOffset );

        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPrecompiledShaderProgramNode" );
        GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vertexShader, geometryShader, fragmentShader, binaryPath, chunk._binaryFormat );
        Add ( *node );
    }
}

GXPrecompiledShaderProgramFinder::~GXPrecompiledShaderProgramFinder ()
{
    // NOTHING
}

GXBool GXPrecompiledShaderProgramFinder::FindProgram ( const GXWChar** binaryPath, GLenum &binaryFormat, const GXWChar* vertexShader, const GXWChar* fragmentShader, const GXWChar* geometryShader ) const
{
    GXPrecompiledShaderProgramNode finderNode;
    GXPrecompiledShaderProgramNode::InitFinderNode ( finderNode, vertexShader, geometryShader, fragmentShader );
    const GXPrecompiledShaderProgramNode* program = static_cast<const GXPrecompiledShaderProgramNode*> ( Find ( finderNode ) );
    GXPrecompiledShaderProgramNode::DestroyFinderNode ( finderNode );

    if ( program )
    {
        *binaryPath = program->_binaryPath;
        binaryFormat = program->_binaryFormat;

        return GX_TRUE;
    }

    *binaryPath = nullptr;
    binaryFormat = GL_INVALID_ENUM;
    return GX_FALSE;
}

GXVoid GXPrecompiledShaderProgramFinder::AddProgram ( const GXWChar* vertexShader, const GXWChar* geometryShader, const GXWChar* fragmentShader, const GXWChar* binaryPath, GLenum binaryFormat )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPrecompiledShaderProgramNode" );
    GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vertexShader, geometryShader, fragmentShader, binaryPath, binaryFormat );
    Add ( *node );

    GXUBigInt size = 0u;
    DoPostfix ( root, &GXPrecompiledShaderProgramFinder::GetDictionarySize, &size );

    size += sizeof ( GXDictionaryHeader );

    GXUByte* dictionary = static_cast<GXUByte*> ( Malloc ( size ) );
    GXDictionaryHeader* header = reinterpret_cast<GXDictionaryHeader*> ( dictionary );
    header->_counter = _counter;
    header->_totalPrecompiledPrograms = totalNodes;
    header->_chunkOffset = sizeof ( GXDictionaryHeader );

    GXSaveState state ( PRECOMPILED_SHADER_PROGRAM_INFO );
    state._content = dictionary;
    state._chunckOffset = header->_chunkOffset;
    state._utf8StringOffset = sizeof ( GXDictionaryHeader ) + header->_totalPrecompiledPrograms * sizeof ( GXChunk );

    DoPostfix ( root, &GXPrecompiledShaderProgramFinder::SaveDictionary, &state );

    if ( !GXWriteToFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, reinterpret_cast<const GXVoid*> ( dictionary ), static_cast<GXUInt> ( size ) ) )
        GXLogA ( "GXPrecompiledShaderProgramFinder::AddProgram::Error - Не могу сохранить файл словаря прекомпилированных шейдерных программ.\n" );

    Free ( dictionary );
}

GXUBigInt GXPrecompiledShaderProgramFinder::GetCounter ()
{
    GXUBigInt c = _counter;
    ++_counter;
    return c;
}

GXVoid GXCALL GXPrecompiledShaderProgramFinder::GetDictionarySize ( const GXAVLTreeNode &node, GXVoid* args )
{
    GXUBigInt* size = static_cast<GXUBigInt*> ( args );
    const GXPrecompiledShaderProgramNode& item = static_cast<const GXPrecompiledShaderProgramNode&> ( node );

    *size += sizeof ( GXChunk );

    GXString tmp;
    GXUPointer stringSize;

    tmp = item._vertexShader;
    tmp.ToUTF8 ( stringSize );
    *size += stringSize;

    if ( item._geometryShader )
    {
        tmp = item._geometryShader;
        tmp.ToUTF8 ( stringSize );
        *size += stringSize;
    }

    if ( item._fragmentShader )
    {
        tmp = item._fragmentShader;
        tmp.ToUTF8 ( stringSize );
        *size += stringSize;
    }

    tmp = item._binaryPath;
    tmp.ToUTF8 ( stringSize );
    *size += stringSize;
};

GXVoid GXCALL GXPrecompiledShaderProgramFinder::SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args )
{
    static const GXUTF8 newLineU[] = "\n";
    static const GXUPointer newLineSize = GXUTF8size ( newLineU ) - 1;

    GXLocale& locale = GXLocale::GetInstance ();

    const GXPrecompiledShaderProgramNode& item = static_cast<const GXPrecompiledShaderProgramNode&> ( node );
    GXSaveState* state = static_cast<GXSaveState*> ( args );
    GXChunk* chunk = reinterpret_cast<GXChunk*> ( state->_content + state->_chunckOffset );

    GXString tmp ( locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Vertex shader: " ) );
    GXUPointer size;

    const GXUTF8* stringU = tmp.ToUTF8 ( size );
    state->_info.Write ( stringU, size - 1u );

    tmp = item._vertexShader;
    stringU = tmp.ToUTF8 ( size );
    state->_info.Write ( stringU, size - 1u );

    memcpy ( state->_content + state->_utf8StringOffset, stringU, size );
    chunk->_vertexShaderOffset = state->_utf8StringOffset;
    state->_utf8StringOffset += size;

    state->_info.Write ( newLineU, newLineSize );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Geometry Shader: " );
    stringU = tmp.ToUTF8 ( size );
    state->_info.Write ( stringU, size - 1u );

    if ( item._geometryShader )
    {
        tmp = item._geometryShader;
        stringU = tmp.ToUTF8 ( size );
        state->_info.Write ( stringU, size - 1u );

        memcpy ( state->_content + state->_utf8StringOffset, stringU, size );
        chunk->_geometryShaderOffset = state->_utf8StringOffset;
        state->_utf8StringOffset += size;

        state->_info.Write ( stringU, size - 1u );
    }
    else
    {
        chunk->_geometryShaderOffset = NULL_STRING_OFFSET;

        tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" );
        stringU = tmp.ToUTF8 ( size );
        state->_info.Write ( stringU, size - 1u );
    }

    state->_info.Write ( newLineU, newLineSize );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Fragment Shader: " );
    stringU = tmp.ToUTF8 ( size );
    state->_info.Write ( stringU, size - 1u );

    if ( item._fragmentShader )
    {
        tmp = item._fragmentShader;
        stringU = tmp.ToUTF8 ( size );
        state->_info.Write ( stringU, size - 1u );

        memcpy ( state->_content + state->_utf8StringOffset, stringU, size );
        chunk->_fragmentShaderOffset = state->_utf8StringOffset;
        state->_utf8StringOffset += size;

        state->_info.Write ( stringU, size - 1u );
    }
    else
    {
        chunk->_fragmentShaderOffset = NULL_STRING_OFFSET;

        tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" );
        stringU = tmp.ToUTF8 ( size );
        state->_info.Write ( stringU, size - 1u );
    }

    state->_info.Write ( newLineU, newLineSize );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Precompiled shader program: " );
    stringU = tmp.ToUTF8 ( size );
    state->_info.Write ( stringU, size - 1u );

    tmp = item._binaryPath;
    stringU = tmp.ToUTF8 ( size );
    memcpy ( state->_content + state->_utf8StringOffset, stringU, size );
    chunk->_binaryPathOffset = state->_utf8StringOffset;
    state->_utf8StringOffset += size;

    state->_info.Write ( stringU, size - 1u );

    state->_info.Write ( newLineU, newLineSize );

    chunk->_binaryFormat = item._binaryFormat;

    state->_chunckOffset += sizeof ( GXChunk );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->-------------------------------------" );
    stringU = tmp.ToUTF8 ( size );
    state->_info.Write ( stringU, size - 1u );

    state->_info.Write ( newLineU, newLineSize );
    state->_info.Write ( newLineU, newLineSize );
}

//---------------------------------------------------------------------------------------------------------------------

class GXShaderProgramEntry final : public GXMemoryInspector
{
    private:
        GXShaderProgramEntry*                       _previous;
        GXShaderProgramEntry*                       _next;

        GXUPointerAtomic                            _references;

        GXWChar*                                    _vertexShader;
        GXWChar*                                    _geometryShader;
        GXWChar*                                    _fragmentShader;

        GLuint                                      _program;

        static GXPrecompiledShaderProgramFinder*    _precompiledShaderProgramFinder;
        static GXString*                            _stringBuffer;
        static GXShaderProgramEntry*                _top;

    public:
        explicit GXShaderProgramEntry ( const GXShaderProgramInfo &info );

        GLuint GetProgram () const;
        GLint GetUniform ( const GLchar* name ) const;

        GXVoid AddReference ();
        GXVoid Release ();

        static GXVoid GXCALL InitPrecompiledShaderProgramSubsystem ();
        static GXVoid GXCALL DestroyPrecompiledShaderProgramSubsystem ();

        // Method returns valid pointer if shader program was found.
        // Method returns nullptr if shader program was not found.
        static GXShaderProgramEntry* GXCALL Find ( const GXShaderProgramInfo &info );

        static GXUInt GXCALL GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );

    private:
        ~GXShaderProgramEntry () override;

        GLuint GetShader ( GLenum type, const GXWChar* fileName );
        GLuint CompileShaderProgram ( const GXShaderProgramInfo &info );
        GXVoid SavePrecompiledShaderProgram ( GLuint shaderProgram, const GXShaderProgramInfo &info, const GXWChar* binaryPath );

        GXShaderProgramEntry () = delete;
        GXShaderProgramEntry ( const GXShaderProgramEntry &other ) = delete;
        GXShaderProgramEntry& operator = ( const GXShaderProgramEntry &other ) = delete;
};

GXPrecompiledShaderProgramFinder*       GXShaderProgramEntry::_precompiledShaderProgramFinder = nullptr;
GXString*                               GXShaderProgramEntry::_stringBuffer = nullptr;
GXShaderProgramEntry*                   GXShaderProgramEntry::_top = nullptr;

GXShaderProgramEntry::GXShaderProgramEntry ( const GXShaderProgramInfo &info )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXShaderProgramEntry" )
    _previous ( nullptr ),
    _next ( _top ),
    _references ( 1u )
{
    if ( _top )
        _top->_previous = this;

    _top = this;

    GXWcsclone ( &_vertexShader, info._vertexShader );

    if ( info._geometryShader )
        GXWcsclone ( &_geometryShader, info._geometryShader );
    else
        _geometryShader = nullptr;

    if ( info._fragmentShader )
        GXWcsclone ( &_fragmentShader, info._fragmentShader );
    else
        _fragmentShader = nullptr;

    GXBool doesPrecompiledShaderProgramExist = GX_FALSE;

    if ( _precompiledShaderProgramFinder )
    {
        const GXWChar* binaryPath = nullptr;
        GLenum binaryFormat = GL_INVALID_ENUM;

        doesPrecompiledShaderProgramExist = _precompiledShaderProgramFinder->FindProgram ( &binaryPath, binaryFormat, _vertexShader, _geometryShader, _fragmentShader );

        if ( doesPrecompiledShaderProgramExist )
        {
            GXUByte* precompiledShaderProgram;
            GXUPointer size = 0u;

            GXFile file ( binaryPath );

            if ( file.LoadContent ( precompiledShaderProgram, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
            {
                _program = glCreateProgram ();
                glProgramBinary ( _program, binaryFormat, precompiledShaderProgram, static_cast<GLsizei> ( size ) );

                file.Close ();

                GLint status = 0;
                glGetProgramiv ( _program, GL_LINK_STATUS, &status );

                if ( status != GL_TRUE )
                {
                    GXLogA ( "GXShaderProgramEntry::GXShaderProgramEntry::Warning - Не могу использовать прекомпилированную шейдерную программу:\n%S\n%S\n%S\n[%S]\n\n", _vertexShader, _geometryShader, _fragmentShader, binaryPath );

                    GXInt logSize = 0;
                    glGetShaderiv ( _program, GL_INFO_LOG_LENGTH, &logSize );

                    if ( logSize > 0 )
                    {
                        GLchar* log = static_cast<GLchar*> ( Malloc ( static_cast<GXUPointer> ( logSize ) ) );
                        GLsizei length;
                        glGetProgramInfoLog ( _program, static_cast<GLsizei> ( logSize ), &length, log );

                        GXLogA ( "%s\n\n", log );

                        Free ( log );
                    }

                    GXLogA ( "Возможно обновился драйвер видеокарты. Пробую сделать перекомпиляцию шейдерной программы...\n\n" );

                    glDeleteProgram ( _program );
                    _program = CompileShaderProgram ( info );

                    if ( _program == INVALID_SHADER_PROGRAM ) return;

                    SavePrecompiledShaderProgram ( _program, info, binaryPath );
                }
            }
            else
            {
                GXLogA ( "GXShaderProgramEntry::GXShaderProgramEntry::Warning - Не могу загрузить файл с прекомпилированной шейдерной программой:\n%S\n%S\n%S\n[%S]\nПопробую сделать перекомпиляцию шейдерной программы...\n\n", _vertexShader, _geometryShader, _fragmentShader, binaryPath );
                _program = CompileShaderProgram ( info );

                if ( _program == INVALID_SHADER_PROGRAM ) return;

                SavePrecompiledShaderProgram ( _program, info, binaryPath );
            }
        }
    }

    if ( !doesPrecompiledShaderProgramExist )
    {
        _program = CompileShaderProgram ( info );

        if ( _program == INVALID_SHADER_PROGRAM ) return;

        if ( _precompiledShaderProgramFinder )
        {
            _stringBuffer->Format ( "%S/%llu.%S", PRECOMPILED_SHADER_PROGRAM_DIRECTORY, _precompiledShaderProgramFinder->GetCounter (), PRECOMPILED_SHADER_PROGRAM_EXTENSION );
            SavePrecompiledShaderProgram ( _program, info, *_stringBuffer );
        }
    }

    glUseProgram ( _program );

    for ( GXUInt i = 0u; i < info._samplers; ++i )
        glUniform1i ( GetUniform ( info._samplerNames[ i ] ), static_cast<GLint> ( info._samplerLocations[ i ] ) );

    glUseProgram ( 0u );
}

GLuint GXShaderProgramEntry::GetProgram () const
{
    return _program;
}

GLint GXShaderProgramEntry::GetUniform ( const GLchar* name ) const
{
    GLint uniform = glGetUniformLocation ( _program, name );

    if ( uniform == -1 )
    {
        GXWChar* nameW;
        GXToWcs ( &nameW, name );
        GXLogA ( "GXShaderProgramEntry::GetUniform::Warning - Не могу найти юниформу %s в программе %S + %S + %S\n", nameW, _vertexShader, _geometryShader, _fragmentShader );
        free ( nameW );
    }

    return uniform;
}

GXVoid GXShaderProgramEntry::AddReference ()
{
    ++_references;
}

GXVoid GXShaderProgramEntry::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXCALL GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem ()
{
    if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPrecompiledShaderProgramFinder" );
    _precompiledShaderProgramFinder = new GXPrecompiledShaderProgramFinder ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXString" );
    _stringBuffer = new GXString ();

    if ( GXDoesDirectoryExist ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

    if ( GXCreateDirectory ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

    GXLogA ( "GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem::Error - Не могу создать директорию для прекомпилированных шейдерных программ [%S]\n", PRECOMPILED_SHADER_PROGRAM_DIRECTORY );
}

GXVoid GXCALL GXShaderProgramEntry::DestroyPrecompiledShaderProgramSubsystem ()
{
    GXSafeDelete ( _stringBuffer );
    GXSafeDelete ( _precompiledShaderProgramFinder );
}

GXShaderProgramEntry* GXCALL GXShaderProgramEntry::Find ( const GXShaderProgramInfo &info )
{
    for ( GXShaderProgramEntry* p = _top; p; p = p->_next )
    {
        if ( GXWcscmp ( p->_fragmentShader, info._fragmentShader ) != 0 ) continue;
        if ( GXWcscmp ( p->_vertexShader, info._vertexShader ) != 0 ) continue;
        if ( GXWcscmp ( p->_geometryShader, info._geometryShader ) != 0 ) continue;

        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXShaderProgramEntry::GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
    GXUInt total = 0u;

    for ( GXShaderProgramEntry* p = _top; p; p = p->_next )
        ++total;

    if ( total > 0u )
    {
        *lastVS = _top->_vertexShader;
        *lastGS = _top->_geometryShader;
        *lastFS = _top->_fragmentShader;
    }
    else
    {
        *lastVS = nullptr;
        *lastGS = nullptr;
        *lastFS = nullptr;
    }

    return total;
}

GXShaderProgramEntry::~GXShaderProgramEntry ()
{
    free ( _vertexShader );
    GXSafeFree ( _geometryShader );
    GXSafeFree ( _fragmentShader );

    glUseProgram ( 0u );
    glDeleteProgram ( _program );

    if ( _top == this )
        _top = _top->_next;

    if ( _previous )
        _previous->_next = _next;

    if ( !_next ) return;

    _next->_previous = _previous;
}

GLuint GXShaderProgramEntry::GetShader ( GLenum type, const GXWChar* fileName )
{
    if ( !fileName ) return 0u;

    GLuint shader = glCreateShader ( type );

    GXUByte* shaderSource = nullptr;
    GXUPointer shaderSourceLength = 0u;

    static const GXMBChar* vertexShaderLoadError = "GXShaderProgramEntry::GetShader::Error - Не могу загрузить вершинный шейдер %S\n";
    static const GXMBChar* geometryShaderLoadError = "GXShaderProgramEntry::GetShader::Error - Не могу загрузить геометрический шейдер %S\n";
    static const GXMBChar* fragmentShaderLoadError = "GXShaderProgramEntry::GetShader::Error - Не могу загрузить фрагментный шейдер %S\n";

    const GXMBChar* loadError = nullptr;

    switch ( type )
    {
        case GL_VERTEX_SHADER:
            loadError = vertexShaderLoadError;
        break;

        case GL_GEOMETRY_SHADER:
            loadError = geometryShaderLoadError;
        break;

        case GL_FRAGMENT_SHADER:
            loadError = fragmentShaderLoadError;
        break;

        default:
            // NOTHING
        break;
    }

    GXFile file ( fileName );

    if ( !file.LoadContent ( shaderSource, shaderSourceLength, eGXFileContentOwner::GXFile, GX_TRUE ) )
    {
        GXLogA ( loadError, fileName );
        glDeleteShader ( shader );
        return 0u;
    }

    GLint sourceSize = static_cast<GLint> ( shaderSourceLength );

    glShaderSource ( shader, 1, reinterpret_cast<const GLchar* const*> ( &shaderSource ), &sourceSize );
    glCompileShader ( shader );
    file.Close ();

    GLint status;

    glGetShaderiv ( shader, GL_COMPILE_STATUS, &status );

    if ( status == GL_TRUE )
    {
        return shader;
    }

    GXInt size = 0;
    glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &size );

    GLchar* log = static_cast<GLchar*> ( Malloc ( static_cast<GXUPointer> ( size ) ) );
    glGetShaderInfoLog ( shader, size, &size, log );

    GXLogA ( "GXShaderProgramEntry::GetShader::Error - Шейдер %S\n", fileName );
    GXLogA ( "%s\n", log );

    Free ( log );

    glDeleteShader ( shader );
    return 0u;
}

GLuint GXShaderProgramEntry::CompileShaderProgram ( const GXShaderProgramInfo &info )
{
    GLuint shaderProgram = glCreateProgram ();
    GLuint vertexShaderObject = GetShader ( GL_VERTEX_SHADER, _vertexShader );
    GLuint geometryShaderObject = INVALID_SHADER;

    if ( _geometryShader )
    {
        geometryShaderObject = GetShader ( GL_GEOMETRY_SHADER, _geometryShader );

        if ( geometryShaderObject == INVALID_SHADER )
        {
            glDeleteProgram ( shaderProgram );
            return INVALID_SHADER_PROGRAM;
        }
    }
    else
    {
        geometryShaderObject = SHADER_NOT_NEEDED;
    }

    GLuint fragmentShaderObject = INVALID_SHADER;

    if ( _fragmentShader )
    {
        fragmentShaderObject = GetShader ( GL_FRAGMENT_SHADER, _fragmentShader );

        if ( fragmentShaderObject == INVALID_SHADER )
        {
            glDeleteProgram ( shaderProgram );
            return INVALID_SHADER_PROGRAM;
        }
    }
    else
    {
        _fragmentShader = SHADER_NOT_NEEDED;
    }

    glAttachShader ( shaderProgram, vertexShaderObject );
    glDeleteShader ( vertexShaderObject );

    if ( _geometryShader )
    {
        glAttachShader ( shaderProgram, geometryShaderObject );
        glDeleteShader ( geometryShaderObject );
    }

    if ( _fragmentShader )
    {
        glAttachShader ( shaderProgram, fragmentShaderObject );
        glDeleteShader ( fragmentShaderObject );
    }

    if ( info._transformFeedbackOutputs > 0 )
        glTransformFeedbackVaryings ( shaderProgram, info._transformFeedbackOutputs, info._transformFeedbackOutputNames, GL_INTERLEAVED_ATTRIBS );

    glLinkProgram ( shaderProgram );

    GLint status = 0;
    glGetProgramiv ( shaderProgram, GL_LINK_STATUS, &status );

    if ( status == GL_TRUE ) return shaderProgram;

    GXInt size = 0;
    glGetShaderiv ( shaderProgram, GL_INFO_LOG_LENGTH, &size );

    GLchar* log = static_cast<GLchar*> ( Malloc ( static_cast<GXUPointer> ( size ) ) );
    glGetProgramInfoLog ( shaderProgram, size, &size, log );

    GXLogA ( "GXShaderProgramEntry::CompileShaderProgram::Error - Не могу слинковать шейдерную программу %S + %S + %S\n", _vertexShader, _geometryShader, _fragmentShader );
    GXLogA ( "%s\n", log );

    Free ( log );

    glDeleteProgram ( shaderProgram );

    return INVALID_SHADER_PROGRAM;
}

GXVoid GXShaderProgramEntry::SavePrecompiledShaderProgram ( GLuint shaderProgram, const GXShaderProgramInfo &info, const GXWChar* binaryPath )
{
    GLint value = static_cast<GLint> ( GL_TRUE );
    glProgramParameteri ( shaderProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, value );

    GLint precompiledShaderProgramSize = 0;
    glGetProgramiv ( shaderProgram, GL_PROGRAM_BINARY_LENGTH, &precompiledShaderProgramSize );

    GXUByte* precompiledShaderProgram = static_cast<GXUByte*> ( Malloc ( static_cast<GXUPointer> ( precompiledShaderProgramSize ) ) );

    GLsizei length;
    GLenum binaryFormat;
    glGetProgramBinary ( shaderProgram, static_cast<GLsizei> ( precompiledShaderProgramSize ), &length, &binaryFormat, precompiledShaderProgram );

    GXWriteToFile ( binaryPath, precompiledShaderProgram, static_cast<GXUPointer> ( length ) );

    _precompiledShaderProgramFinder->AddProgram ( info._vertexShader, info._geometryShader, info._fragmentShader, binaryPath, binaryFormat );

    Free ( precompiledShaderProgram );
}

//---------------------------------------------------------------------------------------------------------------------

GXShaderProgram::GXShaderProgram ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXShaderProgram" )
    _shaderProgramEntry ( nullptr )
{
    // NOTHING
}

GXShaderProgram::~GXShaderProgram ()
{
    if ( !_shaderProgramEntry ) return;

    _shaderProgramEntry->Release ();
}

GXVoid GXShaderProgram::Init ( const GXShaderProgramInfo &info )
{
    if ( _shaderProgramEntry )
        _shaderProgramEntry->Release ();

    _shaderProgramEntry = GXShaderProgramEntry::Find ( info );

    if ( _shaderProgramEntry )
    {
        _shaderProgramEntry->AddReference ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXShaderProgramEntry" );
    _shaderProgramEntry = new GXShaderProgramEntry ( info );
}

GLuint GXShaderProgram::GetProgram () const
{
    return _shaderProgramEntry->GetProgram ();
}

GLint GXShaderProgram::GetUniform ( const GLchar* name ) const
{
    return _shaderProgramEntry->GetUniform ( name );
}

GXVoid GXCALL GXShaderProgram::InitPrecompiledShaderProgramSubsystem ()
{
    GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem ();
}

GXVoid GXCALL GXShaderProgram::DestroyPrecompiledShaderProgramSubsystem ()
{
    GXShaderProgramEntry::DestroyPrecompiledShaderProgramSubsystem ();
}

GXUInt GXCALL GXShaderProgram::GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
    return GXShaderProgramEntry::GetTotalLoadedShaderPrograms ( lastVS, lastGS, lastFS );
}
