//version 1.2

#include <GXEngine/GXOGGSoundProvider.h>


GXUPointer GXOGGRead ( GXVoid* ptr, GXUPointer size, GXUPointer count, GXVoid* datasource )
{
	GXOGGSoundStreamer* streamer = (GXOGGSoundStreamer*)datasource;
	return (GXUPointer)streamer->Read ( ptr, (GXUInt)( size * count ) );
}

GXInt GXOGGSeek ( GXVoid* datasource, ogg_int64_t offset, GXInt whence )
{
	GXOGGSoundStreamer* streamer = (GXOGGSoundStreamer*)datasource;
	return streamer->Seek ( (GXInt)offset, whence );
}

long GXOGGTell ( GXVoid* datasource )
{
	GXOGGSoundStreamer* streamer = (GXOGGSoundStreamer*)datasource;
	return streamer->Tell ();
}

GXInt GXOGGClose ( GXVoid* /*datasource*/ )
{
	return 0;
}

//---------------------------------------------------------------------------------------------------

GXOGGSoundStreamer::GXOGGSoundStreamer ( GXVoid* mappedFile, GXUInt totalSize ):
GXSoundStreamer ( mappedFile, totalSize )
{
	ov_callbacks callbacks;
	callbacks.close_func = &GXOGGClose;
	callbacks.read_func = &GXOGGRead;
	callbacks.seek_func = &GXOGGSeek;
	callbacks.tell_func = &GXOGGTell;

	if ( GXOvOpenCallbacks ( this, &vorbisFile, 0, 0, callbacks ) < 0 )
	{
		GXDebugBox ( L"GXOGGSoundStreamer::Error - не удалось открыть файл" );
	}
}

GXOGGSoundStreamer::~GXOGGSoundStreamer ()
{
	GXOvClear ( &vorbisFile );
}

GXBool GXOGGSoundStreamer::FillBuffer ( ALuint buffer, GXBool isLooped )
{
	GXInt bufferFilling = 0;
	GXInt bitstream;

	while ( bufferFilling < GX_SOUND_PROVIDER_BUFFER_SIZE )
	{
		GXLong decoded = GXOvRead ( &vorbisFile, pcmData + bufferFilling, GX_SOUND_PROVIDER_BUFFER_SIZE - bufferFilling, 0, 2, 1, &bitstream );
		bufferFilling += decoded;

		if ( !decoded )
		{
			if ( !isLooped ) break;
			GXOvPcmSeek ( &vorbisFile, 0 );
		}
	}

	GXAlBufferi ( buffer, AL_FREQUENCY, vorbisFile.vi->rate );
	GXAlBufferi ( buffer, AL_CHANNELS, vorbisFile.vi->channels );
	GXAlBufferData ( buffer, vorbisFile.vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, pcmData, bufferFilling, vorbisFile.vi->rate );
	
	return bufferFilling ? GX_TRUE : GX_FALSE;
}

GXVoid GXOGGSoundStreamer::DecompressAll ( ALuint buffer )
{
	GXUInt decompressedSize = vorbisFile.vi->channels * 2 * (GXUInt)GXOvPcmTotal ( &vorbisFile, -1 );

	GXChar* temp = (GXChar*)malloc ( decompressedSize );
	GXInt bitstream;
	GXInt bufferFilling = 0;

	while ( bufferFilling < (GXLong)decompressedSize )
	{
		GXLong decoded = GXOvRead ( &vorbisFile, temp + bufferFilling, (GXInt)decompressedSize - bufferFilling, 0, 2, 1, &bitstream );
		bufferFilling += decoded;
	}

	GXAlBufferi ( buffer, AL_FREQUENCY, vorbisFile.vi->rate );
	GXAlBufferi ( buffer, AL_CHANNELS, vorbisFile.vi->channels );
	GXAlBufferData ( buffer, vorbisFile.vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, temp, (ALsizei)decompressedSize, vorbisFile.vi->rate );

	free ( temp );
}

//---------------------------------------------------------------------------------------------------

GXOGGSoundTrack::GXOGGSoundTrack ( const GXWChar* trackFile ):
GXSoundTrack ( trackFile )
{
	//NOTHING
}

GXSoundStreamer* GXOGGSoundTrack::GetStreamer ()
{
	readyBuffer = 0;
	return new GXOGGSoundStreamer ( mappedFile, (GXUInt)totalSize ); 
}

ALuint GXOGGSoundTrack::GetBuffer ()
{
	if ( readyBuffer ) return readyBuffer;
	
	GXAlGenBuffers ( 1, &readyBuffer );
	GXOGGSoundStreamer* streamer = new GXOGGSoundStreamer ( mappedFile, (GXUInt)totalSize );
	streamer->DecompressAll ( readyBuffer );
	delete streamer;
	return readyBuffer;
}

GXOGGSoundTrack::~GXOGGSoundTrack ()
{
	if ( !readyBuffer ) GXAlDeleteBuffers ( 1, &readyBuffer );
}