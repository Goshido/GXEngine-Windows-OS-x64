// version 1.6

#ifndef GX_OGG_SOUND_PROVIDER
#define GX_OGG_SOUND_PROVIDER


#include "GXSoundProvider.h"
#include "GXSound.h"


class GXOGGSoundStreamer final : public GXSoundStreamer
{
    private:
        OggVorbis_File      vorbisFile;

    public:
        explicit GXOGGSoundStreamer ( GXVoid* mappedFile, GXUInt totalSize );
        ~GXOGGSoundStreamer () override;

        GXBool FillBuffer ( ALuint buffer, GXBool isLooped ) override;
        GXVoid DecompressAll ( ALuint buffer ) override;

    private:
        GXOGGSoundStreamer () = delete;
        GXOGGSoundStreamer ( const GXOGGSoundStreamer &other ) = delete;
        GXOGGSoundStreamer& operator = ( const GXOGGSoundStreamer &other ) = delete;
};

//----------------------------------------------------------------------------------------------------

class GXOGGSoundTrack final : public GXSoundTrack
{
    public:
        explicit GXOGGSoundTrack ( const GXWChar* trackFile );

        GXSoundStreamer* GetStreamer () override;
        ALuint GetBuffer () override;

    protected:
        ~GXOGGSoundTrack () override;

    private:
        GXOGGSoundTrack () = delete;
        GXOGGSoundTrack ( const GXOGGSoundTrack &other ) = delete;
        GXOGGSoundTrack& operator = ( const GXOGGSoundTrack &other ) = delete;
};


#endif // GX_OGG_SOUND_PROVIDER
