// version 1.0

#ifndef GX_ANIMATION_EXPORTER
#define GX_ANIMATION_EXPORTER


#include "GXExporter.h"
#include "GXSkeleton.h"


class GXAnimationExporter : public GXExporter
{
	private:
		GXSkeleton	skeleton;
		IGameScene*	game;
		IGameNode*	node;
		GXUInt		startFrame;
		GXUInt		lastFrame;
		GXFloat		fps;

	public:
		explicit GXAnimationExporter ( INode& selection, const GXUTF8* fileName, GXUInt startFrame, GXUInt lastFrame );
		~GXAnimationExporter () override;

		GXUTF8* GetWarnings () const override;
		GXUTF8* GetReport () const override;

	protected:
		GXBool Export () override;

	private:
		GXBool Init ();

		GXAnimationExporter ( const GXAnimationExporter &other ) = delete;
		GXAnimationExporter& operator = ( const GXAnimationExporter &other ) = delete;
};


#endif //GX_ANIMATION_EXPORTER
