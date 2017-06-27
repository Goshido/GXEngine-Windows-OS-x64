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

		GXTexture							albedoTexture;
		GXTexture							normalTexture;
		GXTexture							emissionTexture;
		GXTexture							parameterTexture;

		EMCookTorranceCommonPassMaterial	material;

	public:
		EMFluttershy ();
		~EMFluttershy () override;

		GXVoid Render ( GXFloat deltaTime );
		GXVoid UpdatePose ( GXFloat deltaTime );

	protected:
		GXVoid TransformUpdated () override;
};
