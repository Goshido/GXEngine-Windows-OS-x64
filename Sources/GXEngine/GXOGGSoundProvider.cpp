// version 1.6

#include <GXEngine/GXOGGSoundProvider.h>


GXUPointer GXOGGRead ( GXVoid* ptr, GXUPointer size, GXUPointer count, GXVoid* datasource )
{
    GXOGGSoundStreamer* streamer = static_cast<GXOGGSoundStreamer*> ( datasource );
    return static_cast<GXUPointer> ( streamer->Read ( ptr, static_cast<GXUInt> ( size * count ) ) );
}

GXInt GXOGGSeek ( GXVoid* datasource, ogg_int64_t offset, GXInt whence )
{
    GXOGGSoundStreamer* streamer = static_cast<GXOGGSoundStreamer*> ( datasource );
    return streamer->Seek ( static_cast<GXInt> ( offset ), whence );
}

GXLong GXOGGTell ( GXVoid* datasource )
{
    GXOGGSoundStreamer* streamer = static_cast<GXOGGSoundStreamer*> ( datasource );
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

    if ( GXOvOpenCallbacks ( this, &vorbisFile, nullptr, 0, callbacks ) == 0 ) return;

    GXDebugBox ( L"GXOGGSoundStreamer::Error - не удалось открыть файл" );
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
        GXLong decoded = GXOvRead ( &vorbisFile, reinterpret_cast<char*> ( pcmData + bufferFilling ), static_cast<GXInt> ( GX_SOUND_PROVIDER_BUFFER_SIZE ) - bufferFilling, 0, 2, 1, &bitstream );
        bufferFilling += decoded;

        if ( !decoded )
        {
            if ( !isLooped ) break;
            GXOvPcmSeek ( &vorbisFile, 0 );
        }
    }

    GXAlBufferi ( buffer, AL_FREQUENCY, static_cast<ALint> ( vorbisFile.vi->rate ) );
    GXAlBufferi ( buffer, AL_CHANNELS, vorbisFile.vi->channels );
    GXAlBufferData ( buffer, vorbisFile.vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, pcmData, bufferFilling, static_cast<ALsizei> ( vorbisFile.vi->rate ) );
    
    return bufferFilling ? GX_TRUE : GX_FALSE;
}

GXVoid GXOGGSoundStreamer::DecompressAll ( ALuint buffer )
{
    GXUInt decompressedSize = vorbisFile.vi->channels * 2 * static_cast<GXUInt> ( GXOvPcmTotal ( &vorbisFile, -1 ) );

    GXChar* temp = static_cast<GXChar*> ( Malloc ( decompressedSize ) );
    GXInt bitstream;
    GXInt bufferFilling = 0;

    while ( bufferFilling < static_cast<GXLong> ( decompressedSize ) )
    {
        GXLong decoded = GXOvRead ( &vorbisFile, temp + bufferFilling, (GXInt)decompressedSize - bufferFilling, 0, 2, 1, &bitstream );
        bufferFilling += decoded;
    }

    GXAlBufferi ( buffer, AL_FREQUENCY, static_cast<ALint> ( vorbisFile.vi->rate ) );
    GXAlBufferi ( buffer, AL_CHANNELS, vorbisFile.vi->channels );
    GXAlBufferData ( buffer, vorbisFile.vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, temp, static_cast<ALsizei> ( decompressedSize ), static_cast<ALsizei> ( vorbisFile.vi->rate ) );

    Free ( temp );
}

//---------------------------------------------------------------------------------------------------

GXOGGSoundTrack::GXOGGSoundTrack ( const GXWChar* trackFile ):
    GXSoundTrack ( trackFile )
{
    // NOTHING
}

GXSoundStreamer* GXOGGSoundTrack::GetStreamer ()
{
    readyBuffer = 0u;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOGGSoundStreamer" );
    return new GXOGGSoundStreamer ( mappedFile, static_cast<GXUInt> ( totalSize ) );
}

ALuint GXOGGSoundTrack::GetBuffer ()
{
    if ( readyBuffer ) return readyBuffer;
    
    GXAlGenBuffers ( 1, &readyBuffer );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOGGSoundStreamer" );
    GXOGGSoundStreamer* streamer = new GXOGGSoundStreamer ( mappedFile, static_cast<GXUInt> ( totalSize ) );
    streamer->DecompressAll ( readyBuffer );
    delete streamer;
    return readyBuffer;
}

GXOGGSoundTrack::~GXOGGSoundTrack ()
{
    if ( readyBuffer == 0u ) return;
    
    GXAlDeleteBuffers ( 1, &readyBuffer );
}
