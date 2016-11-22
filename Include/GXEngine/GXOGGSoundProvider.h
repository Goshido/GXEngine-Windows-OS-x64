//version 1.2

#ifndef GX_OGG_SOUND_PROVIDER
#define GX_OGG_SOUND_PROVIDER


#include "GXSoundProvider.h"
#include "GXSound.h"


class GXOGGSoundStreamer : public GXSoundStreamer
{
	private:
		OggVorbis_File		vorbisFile;

	public:
		GXOGGSoundStreamer ( GXVoid* mappedFile, GXUInt totalSize );
		virtual ~GXOGGSoundStreamer ();

		virtual GXBool FillBuffer ( ALuint buffer, GXBool isLooped );
		virtual GXVoid DecompressAll ( ALuint buffer );
};

//----------------------------------------------------------------------------------------------------

class GXOGGSoundTrack : public GXSoundTrack
{
	public:
		GXOGGSoundTrack ( const GXWChar* trackFile );

		virtual GXSoundStreamer* GetStreamer ();
		virtual ALuint GetBuffer ();

	protected:
		virtual ~GXOGGSoundTrack ();
};


#endif	//GX_OGG_SOUND_PROVIDER