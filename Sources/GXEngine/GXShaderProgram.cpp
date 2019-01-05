// version 1.9

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
    GXUByte* data;
    GXUPointer size;

    GXFile file ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY );

    if ( !file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
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
        GXLogA ( "GXPrecompiledShaderProgramFinder::AddProgram::Error - Не могу сохранить файл словаря прекомпилированных шейдерных программ.\n" );

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

    GXString tmp;
    GXUPointer stringSize;

    tmp = item.vertexShader;
    tmp.ToUTF8 ( stringSize );
    *size += stringSize;

    if ( item.geometryShader )
    {
        tmp = item.geometryShader;
        tmp.ToUTF8 ( stringSize );
        *size += stringSize;
    }

    if ( item.fragmentShader )
    {
        tmp = item.fragmentShader;
        tmp.ToUTF8 ( stringSize );
        *size += stringSize;
    }

    tmp = item.binaryPath;
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
    GXChunk* chunk = reinterpret_cast<GXChunk*> ( state->content + state->chunckOffset );

    GXString tmp ( locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Vertex shader: " ) );
    GXUPointer size;

    const GXUTF8* stringU = tmp.ToUTF8 ( size );
    state->info.Write ( stringU, size - 1u );

    tmp = item.vertexShader;
    stringU = tmp.ToUTF8 ( size );
    state->info.Write ( stringU, size - 1u );

    memcpy ( state->content + state->utf8StringOffset, stringU, size );
    chunk->vertexShaderOffset = state->utf8StringOffset;
    state->utf8StringOffset += size;

    state->info.Write ( newLineU, newLineSize );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Geometry Shader: " );
    stringU = tmp.ToUTF8 ( size );
    state->info.Write ( stringU, size - 1u );

    if ( item.geometryShader )
    {
        tmp = item.geometryShader;
        stringU = tmp.ToUTF8 ( size );
        state->info.Write ( stringU, size - 1u );

        memcpy ( state->content + state->utf8StringOffset, stringU, size );
        chunk->geometryShaderOffset = state->utf8StringOffset;
        state->utf8StringOffset += size;

        state->info.Write ( stringU, size - 1u );
    }
    else
    {
        chunk->geometryShaderOffset = NULL_STRING_OFFSET;

        tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" );
        stringU = tmp.ToUTF8 ( size );
        state->info.Write ( stringU, size - 1u );
    }

    state->info.Write ( newLineU, newLineSize );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Fragment Shader: " );
    stringU = tmp.ToUTF8 ( size );
    state->info.Write ( stringU, size - 1u );

    if ( item.fragmentShader )
    {
        tmp = item.fragmentShader;
        stringU = tmp.ToUTF8 ( size );
        state->info.Write ( stringU, size - 1u );

        memcpy ( state->content + state->utf8StringOffset, stringU, size );
        chunk->fragmentShaderOffset = state->utf8StringOffset;
        state->utf8StringOffset += size;

        state->info.Write ( stringU, size - 1u );
    }
    else
    {
        chunk->fragmentShaderOffset = NULL_STRING_OFFSET;

        tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" );
        stringU = tmp.ToUTF8 ( size );
        state->info.Write ( stringU, size - 1u );
    }

    state->info.Write ( newLineU, newLineSize );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Precompiled shader program: " );
    stringU = tmp.ToUTF8 ( size );
    state->info.Write ( stringU, size - 1u );

    tmp = item.binaryPath;
    stringU = tmp.ToUTF8 ( size );
    memcpy ( state->content + state->utf8StringOffset, stringU, size );
    chunk->binaryPathOffset = state->utf8StringOffset;
    state->utf8StringOffset += size;

    state->info.Write ( stringU, size - 1u );

    state->info.Write ( newLineU, newLineSize );

    chunk->binaryFormat = item.binaryFormat;

    state->chunckOffset += sizeof ( GXChunk );

    tmp = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->-------------------------------------" );
    stringU = tmp.ToUTF8 ( size );
    state->info.Write ( stringU, size - 1u );

    state->info.Write ( newLineU, newLineSize );
    state->info.Write ( newLineU, newLineSize );
}

//---------------------------------------------------------------------------------------------------------------------

class GXShaderProgramEntry final : public GXMemoryInspector
{
    private:
        GXShaderProgramEntry*                       previous;
        GXShaderProgramEntry*                       next;

        GXUPointerAtomic                            references;

        GXWChar*                                    vertexShader;
        GXWChar*                                    geometryShader;
        GXWChar*                                    fragmentShader;

        GLuint                                      program;

        static GXPrecompiledShaderProgramFinder*    precompiledShaderProgramFinder;
        static GXString*                            stringBuffer;
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
GXString*                               GXShaderProgramEntry::stringBuffer = nullptr;
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

            GXFile file ( binaryPath );

            if ( file.LoadContent ( precompiledShaderProgram, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
            {
                program = glCreateProgram ();
                glProgramBinary ( program, binaryFormat, precompiledShaderProgram, static_cast<GLsizei> ( size ) );

                file.Close ();

                GLint status = 0;
                glGetProgramiv ( program, GL_LINK_STATUS, &status );

                if ( status != GL_TRUE )
                {
                    GXLogA ( "GXShaderProgramEntry::GXShaderProgramEntry::Warning - Не могу использовать прекомпилированную шейдерную программу:\n%S\n%S\n%S\n[%S]\n\n", vertexShader, geometryShader, fragmentShader, binaryPath );

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

                    GXLogA ( "Возможно обновился драйвер видеокарты. Пробую сделать перекомпиляцию шейдерной программы...\n\n" );

                    glDeleteProgram ( program );
                    program = CompileShaderProgram ( info );

                    if ( program == INVALID_SHADER_PROGRAM ) return;

                    SavePrecompiledShaderProgram ( program, info, binaryPath );
                }
            }
            else
            {
                GXLogA ( "GXShaderProgramEntry::GXShaderProgramEntry::Warning - Не могу загрузить файл с прекомпилированной шейдерной программой:\n%S\n%S\n%S\n[%S]\nПопробую сделать перекомпиляцию шейдерной программы...\n\n", vertexShader, geometryShader, fragmentShader, binaryPath );
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
            stringBuffer->Format ( "%S/%llu.%S", PRECOMPILED_SHADER_PROGRAM_DIRECTORY, precompiledShaderProgramFinder->GetCounter (), PRECOMPILED_SHADER_PROGRAM_EXTENSION );
            SavePrecompiledShaderProgram ( program, info, *stringBuffer );
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
        GXLogA ( "GXShaderProgramEntry::GetUniform::Warning - Не могу найти юниформу %s в программе %S + %S + %S\n", nameW, vertexShader, geometryShader, fragmentShader );
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

    if ( references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXVoid GXCALL GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem ()
{
    if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXPrecompiledShaderProgramFinder" );
    precompiledShaderProgramFinder = new GXPrecompiledShaderProgramFinder ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXString" );
    stringBuffer = new GXString ();

    if ( GXDoesDirectoryExist ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

    if ( GXCreateDirectory ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

    GXLogA ( "GXShaderProgramEntry::InitPrecompiledShaderProgramSubsystem::Error - Не могу создать директорию для прекомпилированных шейдерных программ [%S]\n", PRECOMPILED_SHADER_PROGRAM_DIRECTORY );
}

GXVoid GXCALL GXShaderProgramEntry::DestroyPrecompiledShaderProgramSubsystem ()
{
    GXSafeDelete ( stringBuffer );
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

    GXLogA ( "GXShaderProgramEntry::CompileShaderProgram::Error - Не могу слинковать шейдерную программу %S + %S + %S\n", vertexShader, geometryShader, fragmentShader );
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
