#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXAnimSolverPlayer.h>


class EMFluttershy : public GXTransform
{
	private:
		EMMesh								mesh;
		GXSkeleton							skeleton;
		GXAnimSolverPlayer					animationSolverPlayer;
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

	protected:
		GXVoid TransformUpdated () override;
};
