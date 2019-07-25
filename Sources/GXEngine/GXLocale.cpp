// version 1.9

#include <GXEngine/GXLocale.h>
#include <GXCommon/GXAVLTree.h>
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
        GXStringNode ();
        explicit GXStringNode ( const GXUTF8* key, const GXUTF8* value );
        ~GXStringNode () override;

        GXString GetValue () const;

        static GXVoid GXCALL InitFinderNode ( GXStringNode &node, GXString key );
        static GXVoid GXCALL DestroyFinderNode ( GXStringNode &node );

    private:
        GXStringNode ( const GXStringNode &other ) = delete;
        GXStringNode& operator = ( const GXStringNode &other ) = delete;
};

GXStringNode::GXStringNode ()
{
    // NOTHING
}

GXStringNode::GXStringNode ( const GXUTF8* key, const GXUTF8* value )
{
    _key.FromUTF8 ( key );
    _value.FromUTF8 ( value );
}

GXStringNode::~GXStringNode ()
{
    // NOTHING
}

GXString GXStringNode::GetValue () const
{
    return _value;
}

GXVoid GXCALL GXStringNode::InitFinderNode ( GXStringNode &node, GXString key )
{
    node._key = key;
}

GXVoid GXCALL GXStringNode::DestroyFinderNode ( GXStringNode &node )
{
    node._key.Clear ();
    node._value.Clear ();
}

//---------------------------------------------------------------------------------------------------------------------

class GXStringTree final : public GXMemoryInspector, public GXAVLTree
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

GXStringTree::GXStringTree ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXStringTree" )
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
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringNode" );
    GXStringNode* node = new GXStringNode ( key, string );
    Add ( *node );
}

GXString GXStringTree::GetString ( GXString key ) const
{
    GXStringNode finderNode;
    GXStringNode::InitFinderNode ( finderNode, key );
    const GXStringNode* node = static_cast<const GXStringNode*> ( Find ( finderNode ) );
    GXStringNode::DestroyFinderNode ( finderNode );

    if ( node )
        return node->GetValue ();

    return {};
}

eGXCompareResult GXCALL GXStringTree::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    const GXStringNode& aNode = static_cast<const GXStringNode&> ( a );
    const GXStringNode& bNode = static_cast<const GXStringNode&> ( b );
    return aNode._key.Compare ( bNode._key );
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
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXLocale" );
        _instance = new GXLocale ();
    }

    return *_instance;
}

GXLocale::~GXLocale ()
{
    GXUPointer len = _storage.GetLength ();

    if ( len == 0u ) return;

    GXStringTree** trees = static_cast<GXStringTree**> ( _storage.GetData () );

    for ( GXUPointer i = 0u; i < len; ++i )
        GXSafeDelete ( trees[ i ] );

    _instance = nullptr;
}

GXVoid GXLocale::LoadLanguage ( GXString fileName, eGXLanguage language )
{
    GXUByte* rawData;
    GXUPointer size;

    GXFile file ( fileName );
    file.LoadContent ( rawData, size, eGXFileContentOwner::GXFile, GX_TRUE );

    GXUTF8* data = reinterpret_cast<GXUTF8*> ( rawData );

    GXStringTree* tree = nullptr;
    GXVoid* tmp = _storage.GetValue ( static_cast<GXUPointer> ( language ) );

    if ( !tmp )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringTree" );
        tree = new GXStringTree ();
        _storage.SetValue ( static_cast<GXUPointer> ( language ), &tree );
    }
    else
    {
        tree = *( static_cast<GXStringTree**> ( tmp ) );
    }

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

        tree->AddString ( key, string );

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
    GXUPointer languageIndex = static_cast<GXUPointer> ( language );
    GXStringTree** treePointer = static_cast<GXStringTree**> ( _storage.GetValue ( languageIndex ) );

    if ( *treePointer == nullptr )
    {
        GXLogA ( "GXLocale::SetLanguage - Не могу установить язык (не загружен)\n" );
        return;
    }

    _currentLanguage = language;
}

eGXLanguage GXLocale::GetLanguage () const
{
    return _currentLanguage;
}

GXString GXLocale::GetString ( GXString key ) const
{
    GXUPointer languageIndex = static_cast<GXUPointer> ( _currentLanguage );
    GXStringTree** treePointer = static_cast<GXStringTree**> ( _storage.GetValue ( languageIndex ) );
    GXStringTree* tree = *treePointer;

    if ( tree )
        return tree->GetString ( key );

    return {};
}

GXLocale::GXLocale ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXLocale" )
    _currentLanguage ( eGXLanguage::Russian ),
    _storage ( sizeof ( GXStringTree* ) )
{
    // NOTHING
}
