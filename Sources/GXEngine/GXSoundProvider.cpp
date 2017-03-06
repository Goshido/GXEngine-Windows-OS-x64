//version 1.2

#include <GXEngine/GXSoundProvider.h>
#include <GXCommon/GXMemory.h>


GXSoundTrack*	gx_strgSoundTracks = 0;


GXSoundStreamer::GXSoundStreamer ( GXVoid* mappedFile, GXUInt totalSize )
{
	this->mappedFile = (GXChar*)mappedFile;
	this->totalSize = totalSize;
	position = 0;
}

GXSoundStreamer::~GXSoundStreamer ()
{
	//NOTHING
}

GXUInt GXSoundStreamer::Read ( GXVoid* out, GXUInt size )
{
	GXUInt temp = totalSize - position ;
	if ( size > temp ) size = temp;
	
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
			position = totalSize + offset;
		break;
	}

	if ( position < 0 ) position = 0;
	else if ( position > (GXInt)totalSize ) position = totalSize;

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

GXSoundTrack::GXSoundTrack ( const GXWChar* trackFile )
{
	numRef = 1;

	GXUShort size;
	size = (GXUShort)( sizeof ( GXWChar ) * ( wcslen ( trackFile ) + 1 ) );
			
	this->trackFile = (GXWChar*)malloc ( size );
	memcpy ( this->trackFile, trackFile, size );

	if ( !GXLoadFile ( this->trackFile, &mappedFile, totalSize, GX_TRUE ) )
		GXDebugBox ( L"GXSoundTrack::Error - не удалось загрузить файл" );

	readyBuffer = 0;

	prev = 0;
	next = gx_strgSoundTracks;

	if ( next ) next->prev = this;
	gx_strgSoundTracks = this;
}

GXVoid GXSoundTrack::AddRef ()
{
	numRef++;
}

GXVoid GXSoundTrack::Release ()
{
	if ( numRef == 0 )
	{
		GXDebugBox ( L"GXSoundTrack::Error - ѕопытка уменьшить количество ссылок, когда их нет" );
		return;
	}

	numRef--;
	if ( numRef == 0 )
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