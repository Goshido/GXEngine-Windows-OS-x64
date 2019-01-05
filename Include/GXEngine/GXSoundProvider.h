// version 1.7

#ifndef GX_SOUND_PROVIDER
#define GX_SOUND_PROVIDER


#include "GXSound.h"
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXUPointerAtomic.h>


#define GX_SOUND_PROVIDER_BUFFER_SIZE       2097152u    // 2 Mb

//---------------------------------------------------------------------------------------------------------------------

class GXSoundStreamer : public GXMemoryInspector
{
    protected:
        GXUByte*        mappedFile;
        GXUPointer      totalSize;
        GXLong          position;
        GXUByte         pcmData[ GX_SOUND_PROVIDER_BUFFER_SIZE ];

    public:
        explicit GXSoundStreamer ( GXVoid* memoryMappedFile, GXUPointer fileSize );
        ~GXSoundStreamer () override;

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

//---------------------------------------------------------------------------------------------------------------------

class GXSoundTrack : public GXMemoryInspector
{
    public:
        GXSoundTrack*       next;
        GXSoundTrack*       prev;

    protected:
        GXUPointerAtomic    references;
        ALuint              readyBuffer;

    public:
        const GXString      trackFile;

    protected:
        GXUByte*            mappedFile;
        GXUBigInt           totalSize;

    public:
        explicit GXSoundTrack ( const GXWChar* trackFileName );

        GXVoid AddReference ();
        GXVoid Release ();

        virtual GXSoundStreamer* GetStreamer () = 0;
        virtual ALuint GetBuffer () = 0;

    protected:
        ~GXSoundTrack () override;

        GXSoundTrack () = delete;
        GXSoundTrack ( const GXSoundTrack &other ) = delete;
        GXSoundTrack& operator = ( const GXSoundTrack &other ) = delete;
};


#endif //GX_SOUND_PROVIDER
