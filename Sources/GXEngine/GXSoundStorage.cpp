//version 1.5

#include <GXEngine/GXSoundStorage.h>
#include <GXCommon/GXStrings.h>


GXSoundTrack* GXCALL GXGetSoundTrack ( const GXWChar* trackFile )
{
    for ( GXSoundTrack* t = gx_strgSoundTracks; t; t = t->next )
    {
        if ( !GXWcscmp ( t->trackFile, trackFile ) )
        {
            t->AddReference ();
            return t;
        }
    }

    return new GXOGGSoundTrack ( trackFile );
}

GXUInt GXCALL GXGetTotalSoundStorageObjects ( GXWChar** lastSound )
{
    GXUInt objects = 0u;
    *lastSound = nullptr;

    for ( GXSoundTrack* p = gx_strgSoundTracks; p; p = p->next )
    {
        *lastSound = p->trackFile;
        ++objects;
    }

    return objects;
}