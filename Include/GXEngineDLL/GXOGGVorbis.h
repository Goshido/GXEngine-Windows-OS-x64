//version 1.0

#ifndef GX_OGG_VORBIS
#define GX_OGG_VORBIS


#include <vorbis/vorbisfile.h>


typedef int ( *PFNOVOPENCALLBACKS ) ( void *datasource, OggVorbis_File *vf, const char *initial, long ibytes, ov_callbacks callbacks );
typedef	long ( *PFNOVREAD ) ( OggVorbis_File *vf, char *buffer, int length, int bigendianp, int word, int sgned, int *bitstream );
typedef	int ( *PFNOVPCMSEEK ) ( OggVorbis_File *vf, ogg_int64_t pos );
typedef ogg_int64_t ( *PFNOVPCMTOTAL ) ( OggVorbis_File *vf, int i );
typedef	int ( *PFNOVCLEAR ) ( OggVorbis_File *vf );


struct GXOGGVorbisFunctions
{
	PFNOVOPENCALLBACKS*	ov_open_callbacks;
	PFNOVREAD*			ov_read;
	PFNOVPCMSEEK*		ov_pcm_seek;
	PFNOVPCMTOTAL*		ov_pcm_total;
	PFNOVCLEAR*			ov_clear;
};


#endif //GX_OGG_VORBIS