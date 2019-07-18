// version 1.4

#ifndef GX_OGG_VORBIS
#define GX_OGG_VORBIS


#include <GXCommon/GXWarning.h>

GX_DISABLE_COMMON_WARNINGS

#include <vorbis/vorbisfile.h>

GX_RESTORE_WARNING_STATE


typedef int ( *OVOpenCallbacks ) ( void* datasource, OggVorbis_File* vf, const char* initial, long ibytes, ov_callbacks callbacks );
typedef long ( *OVRead ) ( OggVorbis_File* vf, char* buffer, int length, int bigendianp, int word, int sgned, int* bitstream );
typedef int ( *OVPCMSeek ) ( OggVorbis_File* vf, ogg_int64_t pos );
typedef ogg_int64_t ( *OVPCMTotal ) ( OggVorbis_File* vf, int i );
typedef int ( *OVClear ) ( OggVorbis_File* vf );


struct GXOGGVorbisFunctions
{
    OVOpenCallbacks*    ov_open_callbacks;
    OVRead*             ov_read;
    OVPCMSeek*          ov_pcm_seek;
    OVPCMTotal*         ov_pcm_total;
    OVClear*            ov_clear;
};


#endif // GX_OGG_VORBIS
