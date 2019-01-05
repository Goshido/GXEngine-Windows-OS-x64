// version 1.7

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
        GXWChar*    key;
        GXWChar*    value;

    public:
        GXStringNode ();
        explicit GXStringNode ( const GXUTF8* key, const GXUTF8* value );
        ~GXStringNode () override;

        const GXWChar* GetValue () const;

        static GXVoid GXCALL InitFinderNode ( GXStringNode &node, const GXWChar* key );
        static GXVoid GXCALL DestroyFinderNode ( GXStringNode &node );
};

GXStringNode::GXStringNode ()
{
    key = nullptr;
    value = nullptr;
}

GXStringNode::GXStringNode ( const GXUTF8* key, const GXUTF8* value )
{
    GXToWcs ( &this->key, key );
    GXToWcs ( &this->value, value );
}

GXStringNode::~GXStringNode ()
{
    GXSafeFree ( key );
    GXSafeFree ( value );
}

const GXWChar* GXStringNode::GetValue () const
{
    return value;
}

GXVoid GXCALL GXStringNode::InitFinderNode ( GXStringNode &node, const GXWChar* key )
{
    node.key = (GXWChar*)key;
    node.value = nullptr;
}

GXVoid GXCALL GXStringNode::DestroyFinderNode ( GXStringNode &node )
{
    node.key = nullptr;
    node.value = nullptr;
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

    return GXWcscmp ( aNode.key, bNode.key );
}

//---------------------------------------------------------------------------------------------------------------------

enum eGXParserState : GXUByte
{
    Key,
    String
};

//---------------------------------------------------------------------------------------------------------------------

GXLocale* GXLocale::instance = nullptr;

GXLocale& GXCALL GXLocale::GetInstance ()
{
    if ( !instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXLocale" );
        instance = new GXLocale ();
    }

    return *instance;
}

GXLocale::~GXLocale ()
{
    GXUPointer len = storage.GetLength ();

    if ( len == 0u ) return;

    GXStringTree** trees = static_cast<GXStringTree**> ( storage.GetData () );

    for ( GXUPointer i = 0u; i < len; ++i )
        GXSafeDelete ( trees[ i ] );

    instance = nullptr;
}

GXVoid GXLocale::LoadLanguage ( const GXWChar* fileName, eGXLanguage language )
{
    GXUByte* rawData;
    GXUPointer size;

    GXFile file ( fileName );
    file.LoadContent ( rawData, size, eGXFileContentOwner::GXFile, GX_TRUE );

    GXUTF8* data = reinterpret_cast<GXUTF8*> ( rawData );

    GXStringTree* tree = nullptr;
    GXVoid* tmp = storage.GetValue ( static_cast<GXUInt> ( language ) );

    if ( !tmp )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXStringTree" );
        tree = new GXStringTree ();
        storage.SetValue ( static_cast<GXUPointer> ( language ), &tree );
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
    GXStringTree** treePointer = static_cast<GXStringTree**> ( storage.GetValue ( languageIndex ) );

    if ( *treePointer == nullptr )
    {
        GXLogA ( "GXLocale::SetLanguage - Не могу установить язык (не загружен)\n" );
        return;
    }

    currentLanguage = language;
}

eGXLanguage GXLocale::GetLanguage () const
{
    return currentLanguage;
}

const GXWChar* GXLocale::GetString ( const GXWChar* resName ) const
{
    GXUPointer languageIndex = static_cast<GXUPointer> ( currentLanguage );
    GXStringTree** treePointer = static_cast<GXStringTree**> ( storage.GetValue ( languageIndex ) );
    GXStringTree* tree = *treePointer;

    if ( tree )
        return tree->GetString ( resName );

    return nullptr;
}

GXLocale::GXLocale ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXLocale" )
    currentLanguage ( eGXLanguage::Russian ),
    storage ( sizeof ( GXStringTree* ) )
{
    // NOTHING
}
