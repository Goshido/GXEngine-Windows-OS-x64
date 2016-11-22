//version 1.3

#include <GXEngine/GXTGALoader.h>
#include <GXCommon/GXFileSystem.h>


#define TGA_RLE_OFF		2
#define TGA_RLE_ON		10


GXTGAHeader	gx_tgaloader_Header;
GXChar*		gx_tgaloader_FileSource;
GXUInt		gx_tgaloader_FileSize;


GXVoid GXCALL WithoutRLE ( GXUInt &width, GXUInt &height, GXUChar** data )
{
	*data = (GXUChar*)malloc ( gx_tgaloader_Header.Width * gx_tgaloader_Header.Height * gx_tgaloader_Header.BitsPerPixel );
	width = gx_tgaloader_Header.Width;
	height = gx_tgaloader_Header.Height;
	GXUInt index = 0;
	for ( GXUInt i = 0; i < ( GXUInt )( gx_tgaloader_Header.Width * gx_tgaloader_Header.Height ); i++ )
	{
		if ( gx_tgaloader_Header.BitsPerPixel == 32 )
		{
			*( *data + i * 4 )		= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 4 + 2 );	//R
			*( *data + i * 4 + 1 )	= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 4 + 1 );	//G
			*( *data + i * 4 + 2 )	= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 4 );		//B
			*( *data + i * 4 + 3 )	= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 4 + 3);	//A
		}
		else
		{
			*( *data + i * 3 )		= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 3 + 2 );	//R
			*( *data + i * 3 + 1 )	= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 3 + 1 );	//G
			*( *data + i * 3 + 2 )	= *( gx_tgaloader_FileSource + sizeof ( GXTGAHeader ) + i * 3 );		//B
		}
	}
}

void GXCALL WithRLE ( GXUInt* width, GXUInt* height, GXUChar** data )
{
	//TODO
}

GXBool GXCALL GXLoadTGA ( const GXWChar* file_name, GXUInt &width, GXUInt &height, GXBool &bIsAlpha, GXUChar** data )
{
	if ( !GXLoadFile ( file_name, (GXVoid**)&gx_tgaloader_FileSource, gx_tgaloader_FileSize, GX_TRUE ) ) 
	{
		GXDebugBox ( L"GXLoadTGA::Error - не могу загрузить файл" );
		GXLogW ( L"GXLoadTGA::Error - не могу загрузить файл %s\n", file_name );
		return GX_FALSE;
	}
	
	memcpy ( gx_tgaloader_Header.v, gx_tgaloader_FileSource, sizeof ( GXTGAHeader ) );
	switch ( gx_tgaloader_Header.DataType ) 
	{
		case TGA_RLE_OFF:
			WithoutRLE ( width, height, data );
			bIsAlpha = gx_tgaloader_Header.BitsPerPixel == 32;
			GXSafeFree ( gx_tgaloader_FileSource );
		break;

		case TGA_RLE_ON:
			//WithRLE ( width, height, data );
			GXSafeFree ( gx_tgaloader_FileSource );
			return GX_FALSE;
		break;

		default:
			GXSafeFree ( gx_tgaloader_FileSource );
			return GX_FALSE;
		break;
	}

	GXSafeFree ( gx_tgaloader_FileSource );
	return GX_TRUE; 
}