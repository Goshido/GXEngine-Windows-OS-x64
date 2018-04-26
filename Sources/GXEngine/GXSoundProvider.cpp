// version 1.3

#include <GXEngine/GXSoundProvider.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


GXSoundTrack*	gx_strgSoundTracks = nullptr;


GXSoundStreamer::GXSoundStreamer ( GXVoid* mappedFile, GXUPointer totalSize ):
	mappedFile ( static_cast<GXUByte*> ( mappedFile ) ),
	totalSize ( totalSize ),
	position ( 0 )
{
	// NOTHING
}

GXSoundStreamer::~GXSoundStreamer ()
{
	// NOTHING
}

GXUInt GXSoundStreamer::Read ( GXVoid* out, GXUInt size )
{
	GXUInt temp = static_cast<GXUInt> ( totalSize - position );

	if ( size > temp )
		size = temp;
	
	memcpy ( out, mappedFile + position, size );
	position += size;

	return size;
}

GXInt GXSoundStreamer::Seek ( GXInt offset, GXInt whence )
{
	switch ( whence )
	{
		case SEEK_SET:
			position = offset;
		break;

		case SEEK_CUR:
			position += offset;
		break;

		case SEEK_END:
			position = static_cast<GXLong> ( totalSize ) + offset;
		break;
	}

	if ( position < 0 ) 
		position = 0;
	else if ( position > static_cast<GXLong> ( totalSize ) ) 
		position = static_cast<GXLong> ( totalSize );

	return 0;
}

GXLong GXSoundStreamer::Tell ()
{
	return position;
}

GXVoid GXSoundStreamer::Reset ()
{
	position = 0;
}

//-----------------------------------------------------------------------------------------------------

GXSoundTrack::GXSoundTrack ( const GXWChar* trackFile ):
	next ( gx_strgSoundTracks ),
	prev ( nullptr ),
	numRef ( 1u ),
	readyBuffer ( 0u )
{
	if ( next ) next->prev = this;

	gx_strgSoundTracks = this;

	GXWcsclone ( &this->trackFile, trackFile );

	if ( GXLoadFile ( this->trackFile, &mappedFile, totalSize, GX_TRUE ) ) return;
		
	GXDebugBox ( L"GXSoundTrack::Error - не удалось загрузить файл" );
}

GXVoid GXSoundTrack::AddRef ()
{
	numRef++;
}

GXVoid GXSoundTrack::Release ()
{
	if ( numRef < 1u )
	{
		GXDebugBox ( L"GXSoundTrack::Error - ѕопытка уменьшить количество ссылок, когда их нет" );
		return;
	}

	numRef--;

	if ( numRef > 0u ) return;

	delete this;
}

GXSoundTrack::~GXSoundTrack ()
{
	GXSafeFree ( trackFile );
	GXSafeFree ( mappedFile );

	if ( next ) next->prev = prev;

	if ( prev ) 
		prev->next = next;
	else 
		gx_strgSoundTracks = next;
}