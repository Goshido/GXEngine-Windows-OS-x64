// version 1.8

#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXLocale.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXUIntAtomic.h>
#include <GXCommon/GXLogger.h>


#define PRECOMPILED_SHADER_PROGRAM_DICTIONARY       L"Shaders/Precompiled shader program dictionary.bin"
#define PRECOMPILED_SHADER_PROGRAM_INFO             L"Shaders/Precompiled shader programs.info"
#define PRECOMPILED_SHADER_PROGRAM_DIRECTORY        L"Shaders/Precompiled shader programs"
#define PRECOMPILED_SHADER_PROGRAM_EXTENSION        L"spr"

#define NULL_STRING_OFFSET                          0u
#define STRING_BUFFER_SYMBOLS                       260u

#define INVALID_SHADER                              0u
#define SHADER_NOT_NEEDED                           0u
#define INVALID_SHADER_PROGRAM                      0u

//---------------------------------------------------------------------------------------------------------------------

class GXPrecompiledShaderProgramFinder;
class GXPrecompiledShaderProgramNode final : public GXAVLTreeNode
{
    friend class GXPrecompiledShaderProgramFinder;

    private:
        GLenum      binaryFormat;
        GXWChar*    binaryPath;
        GXWChar*    vertexShader;
        GXWChar*    geometryShader;
        GXWChar*    fragmentShader;

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
    binaryPath ( nullptr ),
    binaryFormat ( GL_INVALID_ENUM ),
    vertexShader ( nullptr ),
    geometryShader ( nullptr ),
    fragmentShader ( nullptr )
{
    // NOTHING
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXUTF8* vertexShader, const GXUTF8* geometryShader, const GXUTF8* fragmentShader, const GXUTF8* binaryPath, GLenum binaryFormat ):
    binaryFormat ( binaryFormat )
{
    GXToWcs ( &this->binaryPath, binaryPath );
    GXToWcs ( &this->vertexShader, vertexShader );

    if ( geometryShader )
        GXToWcs ( &this->geometryShader, geometryShader );
    else
        this->geometryShader = nullptr;

    if ( fragmentShader )
    {
        GXToWcs ( &this->fragmentShader, fragmentShader );
        return;
    }

    this->fragmentShader = nullptr;
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXWChar* vertexShader, const GXWChar* geometryShader, const GXWChar* fragmentShader, const GXWChar* binaryPath, GLenum binaryFormat ):
    binaryFormat ( binaryFormat )
{
    GXWcsclone ( &this->binaryPath, binaryPath );
    GXWcsclone ( &this->vertexShader, vertexShader );

    if ( geometryShader )
        GXWcsclone ( &this->geometryShader, geometryShader );
    else
        this->geometryShader = nullptr;

    if ( fragmentShader )
    {
        GXWcsclone ( &this->fragmentShader, fragmentShader );
        return;
    }

    this->fragmentShader = nullptr;
}

GXPrecompiledShaderProgramNode::~GXPrecompiledShaderProgramNode ()
{
    free ( binaryPath );
    free ( vertexShader );
    GXSafeFree ( geometryShader );
    GXSafeFree ( fragmentShader );
}

GXInt GXCALL GXPrecompiledShaderProgramNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXPrecompiledShaderProgramNode& aNode = static_cast<const GXPrecompiledShaderProgramNode&> ( a );
    const GXPrecompiledShaderProgramNode& bNode = static_cast<const GXPrecompiledShaderProgramNode&> ( b );

    GXInt compare = GXWcscmp ( aNode.fragmentShader, bNode.fragmentShader );

    if ( compare != 0 )
        return compare;

    compare = GXWcscmp ( aNode.vertexShader, bNode.vertexShader );

    if ( compare != 0 )
        return compare;

    return GXWcscmp ( aNode.geometryShader, bNode.geometryShader );
}

GXVoid GXCALL GXPrecompiledShaderProgramNode::InitFinderNode ( GXPrecompiledShaderProgramNode& node, const GXWChar* vertexShader, const GXWChar* fragmentShader, const GXWChar* geometryShader )
{
    node.vertexShader = const_cast<GXWChar*> ( vertexShader );
    node.geometryShader = const_cast<GXWChar*> ( geometryShader );
    node.fragmentShader = const_cast<GXWChar*> ( fragmentShader );
    node.binaryPath = nullptr;
}

GXVoid GXCALL GXPrecompiledShaderProgramNode::DestroyFinderNode ( GXPrecompiledShaderProgramNode& node )
{
    node.vertexShader = nullptr;
    node.geometryShader = nullptr;
    node.fragmentShader = nullptr;
    node.binaryPath = nullptr;
}

//---------------------------------------------------------------------------------------------------------------------

#pragma pack ( push )
#pragma pack ( 1 )

struct GXDictionaryHeader final
{
    GXUBigInt       counter;
    GXUInt          totalPrecompiledPrograms;
    GXUBigInt       chunkOffset;
};

struct GXChunk final
{
    GXUBigInt       vertexShaderOffset;
    GXUBigInt       geometryShaderOffset;
    GXUBigInt       fragmentShaderOffset;
    GXUBigInt       binaryPathOffset;
    GLenum          binaryFormat;
};

#pragma pack ( pop )

//---------------------------------------------------------------------------------------------------------------------

class GXSaveState final
{
    public:
        GXWriteFileStream       info;

        GXUByte*                content;
        GXUBigInt               chunckOffset;
        GXUBigInt               utf8StringOffset;

    public:
        explicit GXSaveState ( const GXWChar* infoFile );
        ~GXSaveState ();

    private:
        GXSaveState () = delete;
        GXSaveState ( const GXSaveState &other ) = delete;
        GXSaveState& operator = ( const GXSaveState &other ) = delete;
};

GXSaveState::GXSaveState ( const GXWChar* infoFile ):
    info ( infoFile )
{
    // NOTHING
}

GXSaveState::~GXSaveState ()
{
    info.Close ();
}

//---------------------------------------------------------------------------------------------------------------------

class GXPrecompiledShaderProgramFinder final : public GXMemoryInspector, public GXAVLTree
{
    friend class GXShaderProgramEntry;

    private:
        GXUBigInt       counter;

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
    GXUByte* data = nullptr;
    GXUBigInt size = 0u;

    if ( !GXLoadFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, reinterpret_cast<GXVoid**> ( &data ), size, GX_FALSE ) )
    {
        counter = 0u;
        return;
    }

    GXDictionaryHeader* header = reinterpret_cast<GXDictionaryHeader*> ( data );
    GXChunk* chuncks = reinterpret_cast<GXChunk*> ( data + header->chunkOffset );
    counter = header->counter;

    for ( GXUInt i = 0u; i < header->totalPrecompiledPrograms; ++i )
    {
        const GXChunk& chunk = chuncks[ i ];

        const GXUTF8* vertexShader = chunk.vertexShaderOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk.vertexShaderOffset );
        const GXUTF8* geometryShader = chunk.geometryShaderOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk.geometryShaderOffset );
        const GXUTF8* fragmentShader = chunk.fragmentShaderOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk.fragmentShaderOffset );
        const GXUTF8* binaryPath = reinterpret_cast<const GXUTF8*> ( data + chunk.binaryPathOffset );

        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPrecompiledShaderProgramNode" );
        GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vertexShader, geometryShader, fragmentShader, binaryPath, chunk.binaryFormat );
        Add ( *node );
    }

    free ( data );
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
        *binaryPath = program->binaryPath;
        binaryFormat = program->binaryFormat;

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
    header->counter = counter;
    header->totalPrecompiledPrograms = totalNodes;
    header->chunkOffset = sizeof ( GXDictionaryHeader );

    GXSaveState state ( PRECOMPILED_SHADER_PROGRAM_INFO );
    state.content = dictionary;
    state.chunckOffset = header->chunkOffset;
    state.utf8StringOffset = sizeof ( GXDictionaryHeader ) + header->totalPrecompiledPrograms * sizeof ( GXChunk );

    DoPostfix ( root, &GXPrecompiledShaderProgramFinder::SaveDictionary, &state );

    if ( !GXWriteToFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, reinterpret_cast<const GXVoid*> ( dictionary ), static_cast<GXUInt> ( size ) ) )
        GXLogW ( L"GXPrecompiledShaderProgramFinder::AddProgram::Error - Не могу сохранить файл словаря прекомпилированных шейдерных программ.\n" );

    Free ( dictionary );
}

GXUBigInt GXPrecompiledShaderProgramFinder::GetCounter ()
{
    GXUBigInt c = counter;
    ++counter;
    return c;
}

GXVoid GXCALL GXPrecompiledShaderProgramFinder::GetDictionarySize ( const GXAVLTreeNode &node, GXVoid* args )
{
    GXUBigInt* size = static_cast<GXUBigInt*> ( args );
    const GXPrecompiledShaderProgramNode& item = static_cast<const GXPrecompiledShaderProgramNode&> ( node );

    *size += sizeof ( GXChunk );

    GXUTF8* stringU = nullptr;

    *size += GXToUTF8 ( &stringU, item.vertexShader );;
    free ( stringU );

    if ( item.geometryShader )
    {
        *size += GXToUTF8 ( &stringU, item.geometryShader );
        free ( stringU );
    }

    if ( item.fragmentShader )
    {
        *size += GXToUTF8 ( &stringU, item.fragmentShader );
        free ( stringU );
    }

    *size += GXToUTF8 ( &stringU, item.binaryPath );
    free ( stringU );
};

GXVoid GXCALL GXPrecompiledShaderProgramFinder::SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args )
{
    static const GXUTF8 newLineU[] = "\n";
    static const GXUPointer newLineSize = GXUTF8size ( newLineU ) - 1;

    GXLocale& locale = GXLocale::GetInstance ();

    const GXPrecompiledShaderProgramNode& item = static_cast<const GXPrecompiledShaderProgramNode&> ( node );
    GXSaveState* state = static_cast<GXSaveState*> ( args );
    GXChunk* chunk = reinterpret_cast<GXChunk*> ( state->content + state->chunckOffset );

    GXUTF8* stringU = nullptr;
    GXUPointer size = 0u;

    GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Vertex shader: " ) );
    state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
    free ( stringU );

    size = GXToUTF8 ( &stringU, item.vertexShader );
    memcpy ( state->content + state->utf8StringOffset, stringU, size );
    chunk->vertexShaderOffset = state->utf8StringOffset;
    state->utf8StringOffset += size;

    state->info.Write ( stringU, size - 1u );
    free ( stringU );

    state->info.Write ( newLineU, newLineSize );

    GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Geometry Shader: " ) );
    state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
    free ( stringU );

    if ( item.geometryShader )
    {
        size = GXToUTF8 ( &stringU, item.geometryShader );
        memcpy ( state->content + state->utf8StringOffset, stringU, size );
        chunk->geometryShaderOffset = state->utf8StringOffset;
        state->utf8StringOffset += size;

        state->info.Write ( stringU, size - 1u );
        free ( stringU );
    }
    else
    {
        chunk->geometryShaderOffset = NULL_STRING_OFFSET;

        GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" ) );
        state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
        free ( stringU );
    }

    state->info.Write ( newLineU, newLineSize );

    GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Fragment Shader: " ) );
    state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
    free ( stringU );

    if ( item.fragmentShader )
    {
        size = GXToUTF8 ( &stringU, item.fragmentShader );
        memcpy ( state->content + state->utf8StringOffset, stringU, size );
        chunk->fragmentShaderOffset = state->utf8StringOffset;
        state->utf8StringOffset += size;

        state->info.Write ( stringU, size - 1u );

        free ( stringU );
    }
    else
    {
        chunk->fragmentShaderOffset = NULL_STRING_OFFSET;

        GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" ) );
        state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
        free ( stringU );
    }

    state->info.Write ( newLineU, newLineSize );

    GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Precompiled shader program: " ) );
    state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
    free ( stringU );

    size = GXToUTF8 ( &stringU, item.binaryPath );
    memcpy ( state->content + state->utf8StringOffset, stringU, size );
    chunk->binaryPathOffset = state->utf8StringOffset;
    state->utf8StringOffset += size;

    state->info.Write ( stringU, size - 1u );

    free ( stringU );

    state->info.Write ( newLineU, newLineSize );

    chunk->binaryFormat = item.binaryFormat;

    state->chunckOffset += sizeof ( GXChunk );

    GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->-------------------------------------" ) );
    state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
    free ( stringU );

    state->info.Write ( newLineU, newLineSize );
    state->info.Write ( newLineU, newLineSize );
}

//---------------------------------------------------------------------------------------------------------------------

class GXShaderProgramEntry final : public GXMemoryInspector
{
    private:
        GXShaderProgramEntry*                       previous;
        GXShaderProgramEntry*                       next;

        GXUIntAtomic                                references;

        GXWChar*                                    vertexShader;
        GXWChar*                                    geometryShader;
        GXWChar*                                    fragmentShader;

        GLuint                                      program;

        static GXPrecompiledShaderProgramFinder*    precompiledShaderProgramFinder;
        static GXWChar*                             stringBuffer;
        static GXShaderProgramEntry*                top;

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

GXPrecompiledShaderProgramFinder*       GXShaderProgramEntry::precompiledShaderProgramFinder = nullptr;
GXWChar*                                GXShaderProgramEntry::stringBuffer = nullptr;
GXShaderProgramEntry*                   GXShaderProgramEntry::top = nullptr;

GXShaderProgramEntry::GXShaderProgramEntry ( const GXShaderProgramInfo &info )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXShaderProgramEntry" )
    previous ( nullptr ),
    next ( top ),
    references ( 1u )
{
    if ( top )
        top->previous = this;

    top = this;

    GXWcsclone ( &vertexShader, info.vertexShader );

    if ( info.geometryShader )
        GXWcsclone ( &geometryShader, info.geometryShader );
    else
        geometryShader = nullptr;

    if ( info.fragmentShader )
        GXWcsclone ( &fragmentShader, info.fragmentShader );
    else
        fragmentShader = nullptr;

    GXBool doesPrecompiledShaderProgramExist = GX_FALSE;

    if ( precompiledShaderProgramFinder )
    {
        const GXWChar* binaryPath = nullptr;
        GLenum binaryFormat = GL_INVALID_ENUM;

        doesPrecompiledShaderProgramExist = precompiledShaderProgramFinder->FindProgram ( &binaryPath, binaryFormat, vertexShader, geometryShader, fragmentShader );

        if ( doesPrecompiledShaderProgramExist )
        {
            GXUByte* precompiledShaderProgram;
            GXUPointer size = 0u;

            if ( GXLoadFile ( binaryPath, reinterpret_cast<GXVoid**> ( &precompiledShaderProgram ), size, GX_FALSE ) )
            {
                program = glCreateProgram ();

                glProgramBinary ( program, binaryFormat, precompiledShaderProgram, static_cast<GLsizei> ( size ) );

                free ( precompiledShaderProgram );

                GLint status = 0;
                glGetProgramiv ( program, GL_LINK_STATUS, &status );

                if ( status != GL_TRUE )
                {
                    GXLogW ( L"GXShaderProgramEntry::GXShaderProgramEntry::Warning - Не могу использовать прекомпилированную шейдерную программу:\n%s\n%s\n%s\n[%s]\n\n", vertexShader, geometryShader, fragmentShader, binaryPath );

                    GXInt logSize = 0;
                    glGetShaderiv ( program, GL_INFO_LOG_LENGTH, &logSize );

                    if ( logSize > 0 )
                    {
                        GLchar* log = static_cast<GLchar*> ( Malloc ( static_cast<GXUPointer> ( logSize ) ) );
                        GLsizei length;
                        glGetProgramInfoLog ( program, static_cast<GLsizei> ( logSize ), &length, log );

                        GXLogA ( "%s\n\n", log );

                        Free ( log );
                    }

                    GXLogW ( L"Возможно обновился драйвер видеокарты. Пробую сделать перекомпиляцию шейдерной программы...\n\n" );

                    glDeleteProgram ( program );
                    program = CompileShaderProgram ( info );

                    if ( program == INVALID_SHADER_PROGRAM ) return;

                    SavePrecompiledShaderProgram ( program, info, binaryPath );
                }
            }
            else
            {
                GXLogW ( L"GXShaderProgramEntry::GXShaderProgramEntry::Warning - Не могу загрузить файл с прекомпилированной шейдерной программой:\n%s\n%s\n%s\n[%s]\nПопробую сделать перекомпиляцию шейдерной программы...\n\n", vertexShader, geometryShader, fragmentShader, binaryPath );
                program = CompileShaderProgram ( info );

                if ( program == INVALID_SHADER_PROGRAM ) return;

                SavePrecompiledShaderProgram ( program, info, binaryPath );
            }
        }
    }

    if ( !doesPrecompiledShaderProgramExist )
    {
        program = CompileShaderProgram ( info );

        if ( program == INVALID_SHADER_PROGRAM ) return;

        if ( precompiledShaderProgramFinder )
        {
            swprintf_s ( stringBuffer, STRING_BUFFER_SYMBOLS, L"%s/%llu.%s", PRECOMPILED_SHADER_PROGRAM_DIRECTORY, precompiledShaderProgramFinder->GetCounter (), PRECOMPILED_SHADER_PROGRAM_EXTENSION );
            SavePrecompiledShaderProgram ( program, info, stringBuffer );
        }
    }

    glUseProgram ( program );

    for ( GXUInt i = 0u; i < info.samplers; ++i )
        glUniform1i ( GetUniform ( info.samplerNames[ i ] ), static_cast<GLint> ( info.samplerLocations[ i ] ) );

    glUseProgram ( 0u );
}

GLuint GXShaderProgramEntry::GetProgram () const
{
    return program;
}

GLint GXShaderProgramEntry::GetUniform ( const GLchar* name ) const
{
    GLint uniform = glGetUniformLocation ( program, name );

    if ( uniform == -1 )
    {
        GXWChar* nameW;
        GXToWcs ( &nameW, name );
        GXLogW ( L"GXShaderProgramEntry::GetUniform::Warning - Не могу найти юниформу %s в программе %s + %s + %s\n", nameW, vertexShader, geometryShader, fragmentShader );
        free ( nameW );
    }

    return uniform;
}

GXVoid GXShaderProgramEntry::AddReference ()
{
    ++references;
}

GXVoid GXShaderProgramEntry::Release ()
{
    --references;

    if ( references > 0u ) return;

    delete this;
}

GXVoid GXCALL GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem ()
{
    if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPrecompiledShaderProgramFinder" );
    precompiledShaderProgramFinder = new GXPrecompiledShaderProgramFinder ();
    stringBuffer = static_cast<GXWChar*> ( precompiledShaderProgramFinder->Malloc ( STRING_BUFFER_SYMBOLS * sizeof ( GXWChar ) ) );

    if ( GXDoesDirectoryExist ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

    if ( GXCreateDirectory ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

    GXLogW ( L"GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem::Error - Не могу создать директорию для прекомпилированных шейдерных программ [%s]\n", PRECOMPILED_SHADER_PROGRAM_DIRECTORY );
}

GXVoid GXCALL GXShaderProgramEntry::DestroyPrecompiledShaderProgramSubsystem ()
{
    precompiledShaderProgramFinder->SafeFree ( reinterpret_cast<GXVoid**> ( &stringBuffer ) );
    GXSafeDelete ( precompiledShaderProgramFinder );
}

GXShaderProgramEntry* GXCALL GXShaderProgramEntry::Find ( const GXShaderProgramInfo &info )
{
    for ( GXShaderProgramEntry* p = top; p; p = p->next )
    {
        if ( GXWcscmp ( p->fragmentShader, info.fragmentShader ) != 0 ) continue;
        if ( GXWcscmp ( p->vertexShader, info.vertexShader ) != 0 ) continue;
        if ( GXWcscmp ( p->geometryShader, info.geometryShader ) != 0 ) continue;

        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXShaderProgramEntry::GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
    GXUInt total = 0u;

    for ( GXShaderProgramEntry* p = top; p; p = p->next )
        ++total;

    if ( total > 0u )
    {
        *lastVS = top->vertexShader;
        *lastGS = top->geometryShader;
        *lastFS = top->fragmentShader;
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
    free ( vertexShader );
    GXSafeFree ( geometryShader );
    GXSafeFree ( fragmentShader );

    glUseProgram ( 0u );
    glDeleteProgram ( program );

    if ( top == this )
        top = top->next;

    if ( previous )
        previous->next = next;

    if ( !next ) return;

    next->previous = previous;
}

GLuint GXShaderProgramEntry::GetShader ( GLenum type, const GXWChar* fileName )
{
    if ( !fileName ) return 0u;

    GLuint shader = glCreateShader ( type );

    GXChar* shaderSource = nullptr;
    GXUPointer shaderSourceLength = 0u;

    static const GXWChar* vertexShaderLoadError = L"GXShaderProgramEntry::GetShader::Error - Не могу загрузить вершинный шейдер %s\n";
    static const GXWChar* geometryShaderLoadError = L"GXShaderProgramEntry::GetShader::Error - Не могу загрузить геометрический шейдер %s\n";
    static const GXWChar* fragmentShaderLoadError = L"GXShaderProgramEntry::GetShader::Error - Не могу загрузить фрагментный шейдер %s\n";

    const GXWChar* loadError = nullptr;

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

    if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &shaderSource ), shaderSourceLength, GX_TRUE ) )
    {
        GXLogW ( loadError, fileName );
        glDeleteShader ( shader );
        return 0u;
    }

    GLint sourceSize = static_cast<GLint> ( shaderSourceLength );

    glShaderSource ( shader, 1, static_cast<const GLchar* const*> ( &shaderSource ), &sourceSize );
    glCompileShader ( shader );
    GXSafeFree ( shaderSource );

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

    GXLogW ( L"GXShaderProgramEntry::GetShader::Error - Шейдер %s\n", fileName );
    GXLogA ( "%s\n", log );

    Free ( log );

    glDeleteShader ( shader );
    return 0u;
}

GLuint GXShaderProgramEntry::CompileShaderProgram ( const GXShaderProgramInfo &info )
{
    GLuint shaderProgram = glCreateProgram ();
    GLuint vertexShaderObject = GetShader ( GL_VERTEX_SHADER, vertexShader );
    GLuint geometryShaderObject = INVALID_SHADER;

    if ( geometryShader )
    {
        geometryShaderObject = GetShader ( GL_GEOMETRY_SHADER, geometryShader );

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

    if ( fragmentShader )
    {
        fragmentShaderObject = GetShader ( GL_FRAGMENT_SHADER, fragmentShader );

        if ( fragmentShaderObject == INVALID_SHADER )
        {
            glDeleteProgram ( shaderProgram );
            return INVALID_SHADER_PROGRAM;
        }
    }
    else
    {
        fragmentShader = SHADER_NOT_NEEDED;
    }

    glAttachShader ( shaderProgram, vertexShaderObject );
    glDeleteShader ( vertexShaderObject );

    if ( geometryShader )
    {
        glAttachShader ( shaderProgram, geometryShaderObject );
        glDeleteShader ( geometryShaderObject );
    }

    if ( fragmentShader )
    {
        glAttachShader ( shaderProgram, fragmentShaderObject );
        glDeleteShader ( fragmentShaderObject );
    }

    if ( info.transformFeedbackOutputs > 0 )
        glTransformFeedbackVaryings ( shaderProgram, info.transformFeedbackOutputs, info.transformFeedbackOutputNames, GL_INTERLEAVED_ATTRIBS );

    glLinkProgram ( shaderProgram );

    GLint status = 0;
    glGetProgramiv ( shaderProgram, GL_LINK_STATUS, &status );

    if ( status == GL_TRUE ) return shaderProgram;

    GXInt size = 0;
    glGetShaderiv ( shaderProgram, GL_INFO_LOG_LENGTH, &size );

    GLchar* log = static_cast<GLchar*> ( Malloc ( static_cast<GXUPointer> ( size ) ) );
    glGetProgramInfoLog ( shaderProgram, size, &size, log );

    GXLogW ( L"GXShaderProgramEntry::CompileShaderProgram::Error - Не могу слинковать шейдерную программу %s + %s + %s\n", vertexShader, geometryShader, fragmentShader );
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

    precompiledShaderProgramFinder->AddProgram ( info.vertexShader, info.geometryShader, info.fragmentShader, binaryPath, binaryFormat );

    Free ( precompiledShaderProgram );
}

//---------------------------------------------------------------------------------------------------------------------

GXShaderProgram::GXShaderProgram ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXShaderProgram" )
    shaderProgramEntry ( nullptr )
{
    // NOTHING
}

GXShaderProgram::~GXShaderProgram ()
{
    if ( !shaderProgramEntry ) return;

    shaderProgramEntry->Release ();
}

GXVoid GXShaderProgram::Init ( const GXShaderProgramInfo &info )
{
    if ( shaderProgramEntry )
        shaderProgramEntry->Release ();

    shaderProgramEntry = GXShaderProgramEntry::Find ( info );

    if ( shaderProgramEntry )
    {
        shaderProgramEntry->AddReference ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXShaderProgramEntry" );
    shaderProgramEntry = new GXShaderProgramEntry ( info );
}

GLuint GXShaderProgram::GetProgram () const
{
    return shaderProgramEntry->GetProgram ();
}

GLint GXShaderProgram::GetUniform ( const GLchar* name ) const
{
    return shaderProgramEntry->GetUniform ( name );
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
