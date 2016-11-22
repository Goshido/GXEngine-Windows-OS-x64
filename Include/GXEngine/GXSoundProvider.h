//version 1.2

#ifndef GX_SOUND_PROVIDER
#define GX_SOUND_PROVIDER


#include "GXSound.h"
#include <GXCommon/GXFileSystem.h>


#define GX_SOUND_PROVIDER_BUFFER_SIZE	2097152		//2 мегабайта

class GXSoundStreamer
{
	protected:
		GXChar*		mappedFile;
		GXUInt		totalSize;
		GXLong		position;
		GXChar		pcmData[ GX_SOUND_PROVIDER_BUFFER_SIZE ];

	public:
		GXSoundStreamer ( GXVoid* mappedFile, GXUInt totalSize );
		virtual ~GXSoundStreamer ();

		GXUInt Read ( GXVoid* out, GXUInt size );
		GXInt Seek ( GXInt offset, GXInt whence );
		GXLong Tell ();

		GXVoid Reset ();

		virtual GXBool FillBuffer ( ALuint buffer, GXBool isLooped ) = 0;
		virtual GXVoid DecompressAll ( ALuint buffer ) = 0;
};

//-----------------------------------------------------------------------------------------------------

class GXSoundTrack
{
	public:
		GXSoundTrack*		next;
		GXSoundTrack*		prev;

		GXWChar*			trackFile;

	protected:
		GXUInt				numRef;

		GXVoid*				mappedFile;
		GXUInt				totalSize;

		ALuint				readyBuffer;
		
	public:
		GXSoundTrack ( const GXWChar* trackFile );

		GXVoid AddRef ();
		GXVoid Release ();

		virtual GXSoundStreamer* GetStreamer () = 0;
		virtual ALuint GetBuffer () = 0;

	protected:
		virtual ~GXSoundTrack ();
};


#endif	//GX_SOUND_PROVIDER