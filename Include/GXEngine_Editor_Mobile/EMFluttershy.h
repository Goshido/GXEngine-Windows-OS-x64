#include "EMMesh.h"
#include "EMBlinnPhongCommonPassMaterial.h"
#include <GXEngine/GXAnimSolverPlayer.h>


class EMFluttershy : public GXTransform
{
	private:
		EMMesh							mesh;
		GXSkeleton						skeleton;
		GXAnimSolverPlayer				animationSolverPlayer;
		GXAnimationInfo					animationInfo;

		GXTexture						diffuseTexture;
		GXTexture						normalTexture;
		GXTexture						specularTexture;
		GXTexture						emissionTexture;

		EMBlinnPhongCommonPassMaterial	material;

	public:
		EMFluttershy ();
		~EMFluttershy () override;

		GXVoid Render ();
		GXVoid UpdatePose ( GXFloat deltaTime );

	protected:
		GXVoid TransformUpdated () override;
};
