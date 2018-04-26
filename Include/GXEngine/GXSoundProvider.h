// version 1.3

#ifndef GX_SOUND_PROVIDER
#define GX_SOUND_PROVIDER


#include "GXSound.h"
#include <GXCommon/GXFileSystem.h>


#define GX_SOUND_PROVIDER_BUFFER_SIZE	2097152u		// 2 Mb

class GXSoundStreamer
{
	protected:
		GXUByte*	mappedFile;
		GXUPointer	totalSize;
		GXLong		position;
		GXUByte		pcmData[ GX_SOUND_PROVIDER_BUFFER_SIZE ];

	public:
		explicit GXSoundStreamer ( GXVoid* mappedFile, GXUPointer totalSize );
		virtual ~GXSoundStreamer ();

		GXUInt Read ( GXVoid* out, GXUInt size );
		GXInt Seek ( GXInt offset, GXInt whence );
		GXLong Tell ();

		GXVoid Reset ();

		virtual GXBool FillBuffer ( ALuint buffer, GXBool isLooped ) = 0;
		virtual GXVoid DecompressAll ( ALuint buffer ) = 0;

	private:
		GXSoundStreamer () = delete;
		GXSoundStreamer ( const GXSoundStreamer &other ) = delete;
		GXSoundStreamer& operator = ( const GXSoundStreamer &other ) = delete;
};

//-----------------------------------------------------------------------------------------------------

class GXSoundTrack
{
	public:
		GXSoundTrack*		next;
		GXSoundTrack*		prev;

	protected:
		GXUInt				numRef;
		ALuint				readyBuffer;

	public:
		GXWChar*			trackFile;

	protected:
		GXVoid*				mappedFile;
		GXUBigInt			totalSize;

		
	public:
		explicit GXSoundTrack ( const GXWChar* trackFile );

		GXVoid AddRef ();
		GXVoid Release ();

		virtual GXSoundStreamer* GetStreamer () = 0;
		virtual ALuint GetBuffer () = 0;

	protected:
		virtual ~GXSoundTrack ();

		GXSoundTrack () = delete;
		GXSoundTrack ( const GXSoundTrack &other ) = delete;
		GXSoundTrack& operator = ( const GXSoundTrack &other ) = delete;
};


#endif //GX_SOUND_PROVIDER
