#include <GXEngine/GXLocale.h>
#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>


class GXStringNode : public GXAVLTreeNode
{
	private:
		GXWChar*	key;
		GXWChar*	value;

	public:
		GXStringNode ( const GXUTF8* key, const GXUTF8* value );
		virtual ~GXStringNode ();

		virtual const GXVoid* GetKey ();
		const GXWChar* GetValue ();
};

GXStringNode::GXStringNode ( const GXUTF8* key, const GXUTF8* value )
{
	GXToWcs ( &this->key, key );
	GXToWcs ( &this->value, value );
}

GXStringNode::~GXStringNode ()
{
	free ( key );
	free ( value );
}

const GXVoid* GXStringNode::GetKey ()
{
	return key;
}

const GXWChar* GXStringNode::GetValue ()
{
	return value;
}

//--------------------------------------------------------------------------------

class GXStringTree : public GXAVLTree
{
	public:
		GXStringTree ();
		virtual ~GXStringTree ();

		GXVoid AddString ( const GXUTF8* key, const GXUTF8* string );
		const GXWChar* GetString ( const GXWChar* key );

	private:
		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
		static GXVoid GXCALL Iterator ( const GXAVLTreeNode* node, GXVoid* args );
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
	Add ( node );
}

const GXWChar* GXStringTree::GetString ( const GXWChar* key )
{
	GXStringNode* node = (GXStringNode*)FindByKey ( key );
	if ( node ) return node->GetValue ();
	return 0;
}

GXInt GXCALL GXStringTree::Compare ( const GXVoid* a, const GXVoid* b )
{
	return GXWcscmp ( (const GXWChar*)a, (const GXWChar*)b );
}

//--------------------------------------------------------------------------------

GXLocale::GXLocale ():
storage ( sizeof ( GXStringTree* ) )
{
	language = GX_LANGUAGE_RU;

	GXStringTree* node = 0;
	storage.SetValue ( (GXUInt)GX_LANGUAGE_RU, &node );
	storage.SetValue ( (GXUInt)GX_LANGUAGE_EN, &node );
}

GXLocale::~GXLocale ()
{
	GXUInt len = storage.GetLength ();
	if ( !len ) return;

	for ( GXUInt i = 0; i < len; i++ )
	{
		GXStringTree* tree = *( (GXStringTree**)storage.GetValue ( i ) );
		GXSafeDelete ( tree );
	}
}

GXVoid GXLocale::LoadLanguage ( const GXWChar* fileName, eGXLanguage language )
{
	#define GX_KEY_STATE	0
	#define GX_STRING_STATE	1

	GXUTF8* data;
	GXUInt size;
	GXLoadFile ( fileName, (GXVoid**)&data, size, GX_TRUE );

	GXStringTree* tree = new GXStringTree ();
	storage.SetValue ( (GXUInt)language, &tree );

	GXUByte state = GX_KEY_STATE;
	GXUInt offset = 0;
	GXUInt start = 0;
	GXUTF8* key = data;
	GXUTF8* string = 0;

	while ( offset < size )
	{
		if ( data[ offset ] == '~' )
		{
			data[ offset ] = 0;
			if ( state == GX_KEY_STATE )
			{
				string = data + offset + 1;
				state = GX_STRING_STATE;
			}
			else
			{
				tree->AddString ( key, string );

				while ( 1 )
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
				state = GX_KEY_STATE;
			}
		}

		offset++;
	}

	free ( data );

	#undef GX_KEY_STATE
	#undef GX_STRING_STATE
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

eGXLanguage GXLocale::GetLanguage ()
{
	return language;
}

const GXWChar* GXLocale::GetString ( const GXWChar* resName )
{
	GXStringTree* tree = *( (GXStringTree**)storage.GetValue ( (GXUInt)language ) );
	if ( tree ) return tree->GetString ( resName );
	return 0;
}
