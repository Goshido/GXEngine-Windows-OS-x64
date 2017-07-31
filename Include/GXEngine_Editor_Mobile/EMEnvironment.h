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

	public:
		EMEnvironment ( const GXWChar* environmentMap );
		~EMEnvironment ();

		GXVoid Render ();

		GXVoid OnViewerLocationChanged ();
};


#endif //EM_ENVIRONMENT
