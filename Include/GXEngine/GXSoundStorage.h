//version 1.3

#ifndef GX_SOUND_STORAGE
#define GX_SOUND_STORAGE


#include "GXOGGSoundProvider.h"


extern GXSoundTrack*	gx_strgSoundTracks;

GXSoundTrack* GXCALL GXGetSoundTrack ( const GXWChar* trackFile );
GXUInt GXCALL GXGetTotalSoundStorageObjects ( GXWChar** lastSound );


#endif	//GX_SOUND_STORAGE