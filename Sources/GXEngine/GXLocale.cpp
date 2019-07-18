// version 1.8

#include <GXEngine/GXLocale.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>


#define GX_DEFAULT_SEPARATOR '~'

//---------------------------------------------------------------------------------------------------------------------

class GXStringNode final : public GXAVLTreeNode
{
    public:
        GXWChar*    _key;
        GXWChar*    _value;

    public:
        GXStringNode ();
        explicit GXStringNode ( const GXUTF8* key, const GXUTF8* value );
        ~GXStringNode () override;

        const GXWChar* GetValue () const;

        static GXVoid GXCALL InitFinderNode ( GXStringNode &node, const GXWChar* key );
        static GXVoid GXCALL DestroyFinderNode ( GXStringNode &node );

    private:
        GXStringNode ( const GXStringNode &other ) = delete;
        GXStringNode& operator = ( const GXStringNode &other ) = delete;
};

GXStringNode::GXStringNode ()
{
    _key = nullptr;
    _value = nullptr;
}

GXStringNode::GXStringNode ( const GXUTF8* key, const GXUTF8* value )
{
    GXToWcs ( &_key, key );
    GXToWcs ( &_value, value );
}

GXStringNode::~GXStringNode ()
{
    GXSafeFree ( _key );
    GXSafeFree ( _value );
}

const GXWChar* GXStringNode::GetValue () const
{
    return _value;
}

GXVoid GXCALL GXStringNode::InitFinderNode ( GXStringNode &node, const GXWChar* key )
{
    node._key = const_cast<GXWChar*> ( key );
    node._value = nullptr;
}

GXVoid GXCALL GXStringNode::DestroyFinderNode ( GXStringNode &node )
{
    node._key = nullptr;
    node._value = nullptr;
}

//---------------------------------------------------------------------------------------------------------------------

class GXStringTree final : public GXMemoryInspector, public GXAVLTree
{
    public:
        GXStringTree ();
        ~GXStringTree () override;

        GXVoid AddString ( const GXUTF8* key, const GXUTF8* string );
        const GXWChar* GetString ( const GXWChar* key ) const;

    private:
        static GXInt GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

        GXStringTree ( const GXStringTree &other ) = delete;
        GXStringTree& operator = ( const GXStringTree &other ) = delete;
};

GXStringTree::GXStringTree ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXStringTree" )
    GXAVLTree ( &Compare, GX_TRUE )
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

const GXWChar* GXStringTree::GetString ( const GXWChar* key ) const
{
    GXStringNode finderNode;
    GXStringNode::InitFinderNode ( finderNode, key );
    const GXStringNode* node = static_cast<const GXStringNode*> ( Find ( finderNode ) );
    GXStringNode::DestroyFinderNode ( finderNode );

    if ( node )
        return node->GetValue ();

    return nullptr;
}

GXInt GXCALL GXStringTree::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
    GXStringNode& aNode = (GXStringNode&)a;
    GXStringNode& bNode = (GXStringNode&)b;

    return GXWcscmp ( aNode._key, bNode._key );
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

GXVoid GXLocale::LoadLanguage ( const GXWChar* fileName, eGXLanguage language )
{
    GXUByte* rawData;
    GXUPointer size;

    GXFile file ( fileName );
    file.LoadContent ( rawData, size, eGXFileContentOwner::GXFile, GX_TRUE );

    GXUTF8* data = reinterpret_cast<GXUTF8*> ( rawData );

    GXStringTree* tree = nullptr;
    GXVoid* tmp = _storage.GetValue ( static_cast<GXUInt> ( language ) );

    if ( !tmp )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringTree" );
        tree = new GXStringTree ();
        _storage.SetValue ( static_cast<GXUPointer> ( language ), &tree );
    }
    else
    {
        tree = *( (GXStringTree**)tmp );
    }

    eGXParserState state = eGXParserState::Key;
    GXUInt offset = 0u;
    GXUTF8* key = data;
    GXUTF8* string = nullptr;

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

        while ( GX_TRUE )
        {
            ++offset;

            if ( offset >= size ) break;

            else if ( data[ offset ] != ' ' && data[ offset ] != '\n' && data[ offset ] != '\r' && data[ offset ] != '\t' )
            {
                --offset;
                break;
            }
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

const GXWChar* GXLocale::GetString ( const GXWChar* resName ) const
{
    GXUPointer languageIndex = static_cast<GXUPointer> ( _currentLanguage );
    GXStringTree** treePointer = static_cast<GXStringTree**> ( _storage.GetValue ( languageIndex ) );
    GXStringTree* tree = *treePointer;

    if ( tree )
        return tree->GetString ( resName );

    return nullptr;
}

GXLocale::GXLocale ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXLocale" )
    _currentLanguage ( eGXLanguage::Russian ),
    _storage ( sizeof ( GXStringTree* ) )
{
    // NOTHING
}
