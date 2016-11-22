//version 1.0

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXOGGVorbis.h>


GXDLLEXPORT GXVoid GXCALL GXOGGVorbisInit ( GXOGGVorbisFunctions &out )
{
	*out.ov_clear = &ov_clear;
	*out.ov_open_callbacks = &ov_open_callbacks;
	*out.ov_pcm_seek = &ov_pcm_seek;
	*out.ov_pcm_total = &ov_pcm_total;
	*out.ov_read = &ov_read;
}

