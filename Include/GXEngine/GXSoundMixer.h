// version 1.12

#ifndef GX_SOUND_MIXER
#define GX_SOUND_MIXER


#include "GXSoundChannel.h"
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXThread.h>


class GXSoundMixer final : public GXMemoryInspector
{
    private:
        GXThread                    _thread;
        GXFloat                     _masterVolume;

        static GXBool               _loopFlag;
        static GXSoundChannel*      _channels;
        static GXSoundMixer*        _instance;

    public:
        static GXSoundMixer& GXCALL GetInstance ();
        ~GXSoundMixer () override;

        GXVoid SetListenerVelocity ( const GXVec3 &velocity );
        GXVoid SetListenerVelocity ( GXFloat x, GXFloat y, GXFloat z );

        GXVoid SetListenerLocation ( const GXVec3 &location );
        GXVoid SetListenerLocation ( GXFloat x, GXFloat y, GXFloat z );

        GXVoid SetListenerRotation ( const GXMat4 &rotation );
        GXVoid SetListenerRotation ( const GXEuler &rotation );
        GXVoid SetListenerRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

        GXVoid AddChannel ( GXSoundChannel* channel );

        GXVoid Start ();
        GXBool Shutdown ();

        GXVoid SetMasterVolume ( GXFloat masterVolumeLevel );

    private:
        GXSoundMixer ();

        static GXUPointer GXTHREADCALL Update ( GXVoid* args, GXThread &thread );

        GXSoundMixer ( const GXSoundMixer &other ) = delete;
        GXSoundMixer& operator = ( const GXSoundMixer &other ) = delete;
};


#endif // GX_SOUND_MIXER
