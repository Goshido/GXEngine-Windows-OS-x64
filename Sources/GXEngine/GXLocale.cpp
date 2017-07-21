//version 1.2

#include <GXEngine/GXLocale.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>

#define GX_DEFAULT_SEPARATOR '~'


class GXStringNode : public GXAVLTreeNode
{
	public:
		GXWChar*	key;
		GXWChar*	value;

	public:
		GXStringNode ();
		GXStringNode ( const GXUTF8* key, const GXUTF8* value );
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

//--------------------------------------------------------------------------------

class GXStringTree : public GXAVLTree
{
	public:
		GXStringTree ();
		~GXStringTree () override;

		GXVoid AddString ( const GXUTF8* key, const GXUTF8* string );
		const GXWChar* GetString ( const GXWChar* key ) const;

	private:
		static GXInt GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );
};

GXStringTree::GXStringTree ():
GXAVLTree ( &Compare, GX_TRUE )
{
	//NOTHING
}

GXStringTree::~GXStringTree ()
{
	//NOTHING
}

GXVoid GXStringTree::AddString ( const GXUTF8* key, const GXUTF8* string )
{
	GXStringNode* node = new GXStringNode ( key, string );
	Add ( *node );
}

const GXWChar* GXStringTree::GetString ( const GXWChar* key ) const
{
	GXStringNode finderNode;
	GXStringNode::InitFinderNode ( finderNode, key );
	GXStringNode* node = (GXStringNode*)Find ( finderNode );
	GXStringNode::DestroyFinderNode ( finderNode );

	if ( node )
		return node->GetValue ();

	return 0;
}

GXInt GXCALL GXStringTree::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
	GXStringNode& aNode = (GXStringNode&)a;
	GXStringNode& bNode = (GXStringNode&)b;

	return GXWcscmp ( aNode.key, bNode.key );
}

//--------------------------------------------------------------------------------

enum eGXParserState : GXUByte
{
	Key,
	String
};

//--------------------------------------------------------------------------------

GXLocale* GXLocale::instance = nullptr;

GXLocale::~GXLocale ()
{
	GXUInt len = storage.GetLength ();
	if ( !len ) return;

	for ( GXUInt i = 0; i < len; i++ )
	{
		GXStringTree* tree = *( (GXStringTree**)storage.GetValue ( i ) );
		GXSafeDelete ( tree );
	}

	instance = nullptr;
}

GXVoid GXLocale::LoadLanguage ( const GXWChar* fileName, eGXLanguage language )
{
	GXUTF8* data;
	GXUPointer size;
	GXLoadFile ( fileName, (GXVoid**)&data, size, GX_TRUE );

	GXStringTree* tree = nullptr;
	GXVoid* tmp = storage.GetValue ( (GXUInt)language );

	if ( !tmp )
	{
		tree = new GXStringTree ();
		storage.SetValue ( (GXUInt)language, &tree );
	}
	else
	{
		tree = *( (GXStringTree**)tmp );
	}

	eGXParserState state = eGXParserState::Key;
	GXUInt offset = 0;
	GXUInt start = 0;
	GXUTF8* key = data;
	GXUTF8* string = 0;

	while ( offset < size )
	{
		if ( data[ offset ] == GX_DEFAULT_SEPARATOR )
		{
			data[ offset ] = 0;
			if ( state == eGXParserState::Key )
			{
				string = data + offset + 1;
				state = eGXParserState::String;
			}
			else
			{
				tree->AddString ( key, string );

				while ( GX_TRUE )
				{
					offset++;

					if ( offset >= size ) break;

					else if ( data[ offset ] != ' ' && data[ offset ] != '\n' && data[ offset ] != '\r' && data[ offset ] != '\t' )
					{
						offset--;
						break;
					}
				}

				if ( offset >= size ) break;

				key = data + offset + 1;
				state = eGXParserState::Key;
			}
		}

		offset++;
	}

	free ( data );
}

GXVoid GXLocale::SetLanguage ( eGXLanguage language )
{
	GXStringTree* tree = *( (GXStringTree**)storage.GetValue ( (GXUInt)language ) );
	if ( !tree )
	{
		GXLogW ( L"GXLocale::SetLanguage - Не могу установить язык (не загружен)\n" );
		return;
	}

	this->language = language;
}

eGXLanguage GXLocale::GetLanguage () const
{
	return language;
}

const GXWChar* GXLocale::GetString ( const GXWChar* resName ) const
{
	GXStringTree* tree = *( (GXStringTree**)storage.GetValue ( (GXUInt)language ) );
	if ( tree ) return tree->GetString ( resName );
	return nullptr;
}

GXLocale& GXCALL GXLocale::GetInstance ()
{
	if ( !instance )
		instance = new GXLocale ();

	return *instance;
}

GXLocale::GXLocale ():
storage ( sizeof ( GXStringTree* ) )
{
	language = eGXLanguage::Russian;
}
