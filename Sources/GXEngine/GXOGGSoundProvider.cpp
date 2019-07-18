// version 1.7

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

    if ( GXOvOpenCallbacks ( this, &_vorbisFile, nullptr, 0, callbacks ) == 0 ) return;

    GXWarningBox ( L"GXOGGSoundStreamer::Error - не удалось открыть файл" );
}

GXOGGSoundStreamer::~GXOGGSoundStreamer ()
{
    GXOvClear ( &_vorbisFile );
}

GXBool GXOGGSoundStreamer::FillBuffer ( ALuint buffer, GXBool isLooped )
{
    GXInt bufferFilling = 0;
    GXInt bitstream;

    while ( bufferFilling < GX_SOUND_PROVIDER_BUFFER_SIZE )
    {
        GXLong decoded = GXOvRead ( &_vorbisFile, reinterpret_cast<char*> ( _pcmData + bufferFilling ), static_cast<GXInt> ( GX_SOUND_PROVIDER_BUFFER_SIZE ) - bufferFilling, 0, 2, 1, &bitstream );
        bufferFilling += decoded;

        if ( !decoded )
        {
            if ( !isLooped ) break;
            GXOvPcmSeek ( &_vorbisFile, 0 );
        }
    }

    GXAlBufferi ( buffer, AL_FREQUENCY, static_cast<ALint> ( _vorbisFile.vi->rate ) );
    GXAlBufferi ( buffer, AL_CHANNELS, _vorbisFile.vi->channels );
    GXAlBufferData ( buffer, _vorbisFile.vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, _pcmData, bufferFilling, static_cast<ALsizei> ( _vorbisFile.vi->rate ) );
    
    return bufferFilling ? GX_TRUE : GX_FALSE;
}

GXVoid GXOGGSoundStreamer::DecompressAll ( ALuint buffer )
{
    GXUInt decompressedSize = _vorbisFile.vi->channels * 2 * static_cast<GXUInt> ( GXOvPcmTotal ( &_vorbisFile, -1 ) );

    GXChar* temp = static_cast<GXChar*> ( Malloc ( decompressedSize ) );
    GXInt bitstream;
    GXInt bufferFilling = 0;

    while ( bufferFilling < static_cast<GXLong> ( decompressedSize ) )
    {
        GXLong decoded = GXOvRead ( &_vorbisFile, temp + bufferFilling, (GXInt)decompressedSize - bufferFilling, 0, 2, 1, &bitstream );
        bufferFilling += decoded;
    }

    GXAlBufferi ( buffer, AL_FREQUENCY, static_cast<ALint> ( _vorbisFile.vi->rate ) );
    GXAlBufferi ( buffer, AL_CHANNELS, _vorbisFile.vi->channels );
    GXAlBufferData ( buffer, _vorbisFile.vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, temp, static_cast<ALsizei> ( decompressedSize ), static_cast<ALsizei> ( _vorbisFile.vi->rate ) );

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
    _readyBuffer = 0u;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOGGSoundStreamer" );
    return new GXOGGSoundStreamer ( _mappedFile, static_cast<GXUInt> ( _totalSize ) );
}

ALuint GXOGGSoundTrack::GetBuffer ()
{
    if ( _readyBuffer ) return _readyBuffer;
    
    GXAlGenBuffers ( 1, &_readyBuffer );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOGGSoundStreamer" );
    GXOGGSoundStreamer* streamer = new GXOGGSoundStreamer ( _mappedFile, static_cast<GXUInt> ( _totalSize ) );
    streamer->DecompressAll ( _readyBuffer );
    delete streamer;
    return _readyBuffer;
}

GXOGGSoundTrack::~GXOGGSoundTrack ()
{
    if ( _readyBuffer == 0u ) return;
    
    GXAlDeleteBuffers ( 1, &_readyBuffer );
}
