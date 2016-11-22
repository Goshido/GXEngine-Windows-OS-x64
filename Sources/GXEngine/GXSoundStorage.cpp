//version 1.3

#include <GXEngine/GXSoundStorage.h>
#include <GXCommon/GXStrings.h>


GXSoundTrack* GXCALL GXGetSoundTrack ( const GXWChar* trackFile )
{
	for ( GXSoundTrack* t = gx_strgSoundTracks; t; t = t->next )
	{
		if ( !GXWcscmp ( t->trackFile, trackFile ) )
		{
			t->AddRef ();
			return t;
		}
	}

	return new GXOGGSoundTrack ( trackFile );
}

GXUInt GXCALL GXGetTotalSoundStorageObjects ( GXWChar** lastSound )
{
	GXUInt objects = 0;
	*lastSound = 0;

	for ( GXSoundTrack* p = gx_strgSoundTracks; p; p = p->next )
	{
		*lastSound = p->trackFile;
		objects++;
	}

	return objects;
}