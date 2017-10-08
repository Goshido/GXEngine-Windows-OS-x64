// version 1.0

#ifndef GX_ANIMATION_EXPORTER
#define GX_ANIMATION_EXPORTER


#include "GXSkeleton.h"
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGame.h>
#include <GXCommon/GXRestoreWarnings.h>


class GXAnimationExporter
{
	private:
		GXSkeleton	skeleton;
		IGameScene*	game;
		IGameNode*	node;

	public:
		explicit GXAnimationExporter ( INode& selection, const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame );
		~GXAnimationExporter ();

	private:
		GXBool Init ( INode& selection );
		GXVoid Save ( const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame );
};


#endif //GX_ANIMATION_EXPORTER
