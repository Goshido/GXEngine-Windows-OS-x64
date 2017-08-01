#ifndef EM_ENVIRONMENT
#define EM_ENVIRONMENT


#include "EMMesh.h"
#include "EMEnvironmentMapMaterial.h"
#include <GXEngine/GXTextureCubeMap.h>


class EMEnvironment
{
	private:
		EMMesh						cube;
		GXTextureCubeMap			environment;
		EMEnvironmentMapMaterial	environmentMapMaterial;

		static EMEnvironment*		instance;

	public:
		static EMEnvironment& GXCALL GetInstance ();
		~EMEnvironment ();

		GXVoid SetEnvironmentMap ( const GXWChar* cubeMap );
		GXVoid SetEnvironmentQuasiDistance ( GXFloat meters );

		GXVoid Render ( GXFloat deltaTime );
		GXVoid OnViewerLocationChanged ();

	private:
		EMEnvironment ();
};


#endif //EM_ENVIRONMENT
