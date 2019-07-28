// version 1.10

#include <GXEngine/GXLocale.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>


#define GX_DEFAULT_SEPARATOR '~'

//---------------------------------------------------------------------------------------------------------------------

class GXStringNode final : public GXAVLTreeNode
{
    public:
        GXString    _key;
        GXString    _value;

    public:
        explicit GXStringNode ( const GXUTF8* key, const GXUTF8* value );

        // Constructor for probe object.
        explicit GXStringNode ( GXString key );

        ~GXStringNode () override;

    private:
        GXStringNode () = delete;
        GXStringNode ( const GXStringNode &other ) = delete;
        GXStringNode& operator = ( const GXStringNode &other ) = delete;
};

GXStringNode::GXStringNode ( const GXUTF8* key, const GXUTF8* value )
{
    _key.FromUTF8 ( key );
    _value.FromUTF8 ( value );
}

GXStringNode::GXStringNode ( GXString key ):
    _key ( key )
{
    // NOTHING
}

GXStringNode::~GXStringNode ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXStringTree final : public GXAVLTree
{
    public:
        GXStringTree ();
        ~GXStringTree () override;

        GXVoid AddString ( const GXUTF8* key, const GXUTF8* value );
        GXString GetString ( GXString key ) const;

    private:
        static eGXCompareResult GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

        GXStringTree ( const GXStringTree &other ) = delete;
        GXStringTree& operator = ( const GXStringTree &other ) = delete;
};

GXStringTree::GXStringTree ():
    GXAVLTree ( &GXStringTree::Compare, GX_TRUE )
{
    // NOTHING
}

GXStringTree::~GXStringTree ()
{
    // NOTHING
}

GXVoid GXStringTree::AddString ( const GXUTF8* key, const GXUTF8* string )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringNode" )
    Add ( *( new GXStringNode ( key, string ) ) );
}

GXString GXStringTree::GetString ( GXString key ) const
{
    const GXStringNode* node = static_cast<const GXStringNode*> ( Find ( GXStringNode ( key ) ) );

    if ( node )
        return node->_value;

    GXLogA ( "GXStringTree::GetString::Warning - Can't find key: %s.\n", static_cast<const GXMBChar*> ( key ) );
    return key;
}

eGXCompareResult GXCALL GXStringTree::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXStringNode& aNode = static_cast<const GXStringNode&> ( a );
    const GXStringNode& bNode = static_cast<const GXStringNode&> ( b );
    return aNode._key.Compare ( bNode._key );
}

//---------------------------------------------------------------------------------------------------------------------

class GXLanguageNode final : public GXAVLTreeNode
{
    public:
        eGXLanguage     _language;
        GXStringTree    _stringTree;

    public:
        explicit GXLanguageNode ( eGXLanguage language );
        ~GXLanguageNode () override;

        // Do not delete this object. Create probeMemory in stack as simple GXUByte array
        // size of GXLanguageNode bytes. Method returns probe object reference for convenience.
        static const GXLanguageNode& GXCALL CreateProbe ( GXUByte* probeMemory, eGXLanguage language );

    private:
        GXLanguageNode () = delete;
        GXLanguageNode ( const GXLanguageNode &other ) = delete;
        GXLanguageNode& operator = ( const GXLanguageNode &other ) = delete;
};

GXLanguageNode::GXLanguageNode ( eGXLanguage language ):
    _language ( language )
{
    // NOTHING
}

GXLanguageNode::~GXLanguageNode ()
{
    // NOTHING
}

const GXLanguageNode& GXCALL GXLanguageNode::CreateProbe ( GXUByte* probeMemory, eGXLanguage language )
{
    GXLanguageNode* probe = reinterpret_cast<GXLanguageNode*> ( probeMemory );
    probe->_language = language;
    return *probe;
}

//---------------------------------------------------------------------------------------------------------------------

GXLanguageTree::GXLanguageTree ():
    GXAVLTree ( &GXLanguageTree::Compare, GX_TRUE )
{
    // NOTHING
}

GXLanguageTree::~GXLanguageTree ()
{
    // NOTHING
}

GXLanguageNode& GXLanguageTree::GetLanguage ( eGXLanguage language )
{
    GXUByte probeMemory[ sizeof ( GXLanguageNode ) ];
    const GXAVLTreeNode* node = Find ( GXLanguageNode::CreateProbe ( probeMemory, language ) );

    if ( node )
        return const_cast<GXLanguageNode&> ( *( static_cast<const GXLanguageNode*> ( node ) ) );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXLanguageNode" )
    GXLanguageNode* newNode = new GXLanguageNode ( language );
    Add ( *newNode );

    return *newNode;
}

eGXCompareResult GXCALL GXLanguageTree::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXLanguageNode& aNode = static_cast<const GXLanguageNode&> ( a );
    const GXLanguageNode& bNode = static_cast<const GXLanguageNode&> ( b );

    if ( aNode._language > bNode._language )
        return eGXCompareResult::Greater;

    return aNode._language > bNode._language ? eGXCompareResult::Less : eGXCompareResult::Equal;
}

//---------------------------------------------------------------------------------------------------------------------

enum eGXParserState : GXUByte
{
    Key,
    String
};

//---------------------------------------------------------------------------------------------------------------------

GXLocale* GXLocale::_instance = nullptr;

GXLocale& GXCALL GXLocale::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXLocale" )
        _instance = new GXLocale ();
    }

    return *_instance;
}

GXLocale::~GXLocale ()
{
    _instance = nullptr;
}

GXVoid GXLocale::LoadLanguage ( GXString fileName, eGXLanguage language )
{
    GXUByte* rawData;
    GXUPointer size;

    GXFile file ( fileName );
    file.LoadContent ( rawData, size, eGXFileContentOwner::GXFile, GX_TRUE );

    GXUTF8* data = reinterpret_cast<GXUTF8*> ( rawData );

    GXLanguageNode& node = _languages.GetLanguage ( language );
    GXStringTree& stringTree = node._stringTree;

    eGXParserState state = eGXParserState::Key;
    GXUPointer offset = 0u;
    const GXUTF8* key = data;
    const GXUTF8* string = nullptr;

    while ( offset < size )
    {
        if ( data[ offset ] != GX_DEFAULT_SEPARATOR )
        {
            ++offset;
            continue;
        }

        data[ offset ] = 0;

        if ( state == eGXParserState::Key )
        {
            string = data + offset + 1u;
            state = eGXParserState::String;

            ++offset;
            continue;
        }

        stringTree.AddString ( key, string );

        for ( ; ; )
        {
            ++offset;

            if ( offset >= size ) break;

            const GXUTF8 c = data[ offset ];

            if ( c == ' ' || c == '\n' || c == '\r' || c == '\t' ) continue;

            --offset;
            break;
        }

        if ( offset >= size ) break;

        key = data + offset + 1u;
        state = eGXParserState::Key;

        ++offset;
    }
}

GXVoid GXLocale::SetLanguage ( eGXLanguage language )
{
    _currentLanguage = &_languages.GetLanguage ( language );
}

eGXLanguage GXLocale::GetLanguage () const
{
    return _currentLanguage->_language;
}

GXString GXLocale::GetString ( GXString key ) const
{
    if ( _currentLanguage )
        return _currentLanguage->_stringTree.GetString ( key );

    GXLogA ( "GXLocale::GetString::Warning - Language is not selected. Returning key: %s.\n", static_cast<const GXMBChar*> ( key ) );
    return key;
}

GXLocale::GXLocale ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXLocale" )
    _currentLanguage ( nullptr )
{
    // NOTHING
}
