//version 1.5

#include <GXEngine/GXFontStorage.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


class GXFontUnit
{
	private:
		GXFontUnit*		prev;
		GXFontUnit*		next;

		GXUInt			refs;

		GXWChar*		fileName;
		GXVoid*			mappedFile;
		GXUInt			totalSize;

		FT_Face			face;
		GXUShort		size;
		GXBool			valid;

		GXFont*			font;

	public:
		GXFontUnit ( const GXWChar* fileName, GXUShort size );

		GXVoid AddRef ();
		GXVoid Release ();

		GXFontUnit* Find ( const GXWChar* fileName, GXUShort size );
		GXFontUnit* Find ( const GXFont* font );

		GXFont* GetFont ();
		GXUInt CheckMemoryLeak ( const GXWChar** lastFont );

	private:
		~GXFontUnit ();
};

GXFontUnit*		gx_strg_FontsExt = 0;

GXFontUnit::GXFontUnit ( const GXWChar* fileName, GXUShort size )
{
	if ( !gx_strg_FontsExt )
		next = prev = 0;
	else
	{
		next = gx_strg_FontsExt;
		prev = 0;
		gx_strg_FontsExt->prev = this;
	}

	gx_strg_FontsExt = this;

	refs = 1;
	this->size = size;
	font = 0;
	GXWcsclone ( &this->fileName, fileName );

	if ( !GXLoadFile ( fileName, &mappedFile, totalSize, GX_TRUE ) )
	{
		GXLogA ( "GXFontUnit::GXFontUnit::Error - Can't load font file %s\n", fileName );
		valid = GX_FALSE; 
	}
	else
	{
		if ( GXFtNewMemoryFace ( gx_ft_Library, (FT_Byte*)mappedFile, totalSize, 0, &face ) )
		{
			GXLogA ( "GXFontUnit::GXFontUnit::Error - GXFtNewMemoryFace for file %s failed\n", fileName );
			valid = GX_FALSE;
		}
		else
		{
			valid = GX_TRUE;
			font = new GXFont ( face, size );
		}
	}
}

GXVoid GXFontUnit::AddRef ()
{
	refs++;
}

GXVoid GXFontUnit::Release ()
{
	refs--;

	if ( refs < 1 )
		delete this;
}

GXFontUnit* GXFontUnit::Find ( const GXWChar* fileName, GXUShort size )
{
	GXFontUnit* p = this;

	while ( p )
	{
		if ( ( GXWcscmp ( p->fileName, fileName ) == 0 ) && ( p->size == size ) )
			return p;

		p = p->next;
	}

	return 0;
}

GXFontUnit* GXFontUnit::Find ( const GXFont* font )
{
	GXFontUnit* p = this;

	while ( p )
	{
		if ( p->font == font )
			return p;

		p = p->next;
	}

	return 0;
}

GXFont* GXFontUnit::GetFont ()
{
	return font;
}

GXUInt GXFontUnit::CheckMemoryLeak ( const GXWChar** lastFont )
{
	GXUInt objects = 0;
	*lastFont = 0;

	for ( GXFontUnit* p = gx_strg_FontsExt; p; p = p->next )
	{
		*lastFont = p->fileName;
		objects++;
	}

	return objects;
}

GXFontUnit::~GXFontUnit ()
{
	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		gx_strg_FontsExt = next; 

	if ( valid && GXFtDoneFace ( face ) )
		GXLogA ( "GXFontUnit::~GXFontUnit::Error - FT_Face delete for font %s failed", fileName );

	GXSafeFree ( fileName );
	GXSafeFree ( mappedFile );

	GXSafeDelete ( font );
}

//---------------------------------------------------------------------------------------------

GXFont* GXCALL GXGetFont ( const GXWChar* fileName, GXUShort size )
{
	if ( gx_strg_FontsExt )
	{
		GXFontUnit* font = gx_strg_FontsExt->Find ( fileName, size );
		if ( font )
		{
			font->AddRef ();
			return font->GetFont ();
		}
	}

	GXFontUnit* font = new GXFontUnit ( fileName, size );

	return font->GetFont ();
}

GXVoid GXCALL GXRemoveFont ( const GXFont* font )
{
	if ( !gx_strg_FontsExt ) return;

	GXFontUnit* unit = gx_strg_FontsExt->Find ( font );

	if ( !unit ) return;

	unit->Release ();
}

GXUInt GXCALL GXGetTotalFontStorageObjects ( const GXWChar** lastFont )
{
	if ( !gx_strg_FontsExt )
	{
		*lastFont = 0;
		return 0;
	}

	return gx_strg_FontsExt->CheckMemoryLeak ( lastFont );
}
