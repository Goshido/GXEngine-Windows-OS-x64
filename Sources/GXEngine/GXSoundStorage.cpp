//version 1.6

#include <GXEngine/GXSoundStorage.h>
#include <GXCommon/GXStrings.h>


GXSoundTrack* GXCALL GXGetSoundTrack ( const GXWChar* trackFile )
{
    for ( GXSoundTrack* t = gx_strgSoundTracks; t; t = t->_next )
    {
        if ( !GXWcscmp ( t->_trackFile, trackFile ) )
        {
            t->AddReference ();
            return t;
        }
    }

    return new GXOGGSoundTrack ( trackFile );
}

GXUInt GXCALL GXGetTotalSoundStorageObjects ( const GXWChar** lastSound )
{
    GXUInt objects = 0u;
    *lastSound = nullptr;

    for ( GXSoundTrack* p = gx_strgSoundTracks; p; p = p->_next )
    {
        *lastSound = p->_trackFile;
        ++objects;
    }

    return objects;
}
