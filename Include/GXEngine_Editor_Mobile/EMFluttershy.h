#ifndef EM_FLUTTERSHY
#define EM_FLUTTERSHY


#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXAnimationSolverPlayer.h>


class EMFluttershy : public GXTransform
{
	private:
		EMMesh								mesh;
		GXSkeleton							skeleton;
		GXAnimationSolverPlayer				animationSolverPlayer;
		GXAnimationInfo						animationInfo;

		GXTexture2D							albedoTexture;
		GXTexture2D							normalTexture;
		GXTexture2D							emissionTexture;
		GXTexture2D							parameterTexture;

		EMCookTorranceCommonPassMaterial	material;

	public:
		EMFluttershy ();
		~EMFluttershy () override;

		GXVoid Render ( GXFloat deltaTime );
		GXVoid UpdatePose ( GXFloat deltaTime );

		const GXSkeleton& GetSkeleton () const;

	protected:
		GXVoid TransformUpdated () override;
};


#endif // EM_FLUTTERSHY
