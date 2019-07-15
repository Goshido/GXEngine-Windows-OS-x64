#ifndef EM_ENVIRONMENT
#define EM_ENVIRONMENT


#include "EMMesh.h"
#include "EMEnvironmentMapMaterial.h"
#include <GXEngine/GXTextureCubeMap.h>


class EMEnvironment final
{
    private:
        EMMesh                      _cube;
        GXTextureCubeMap*           _environment;

        EMEnvironmentMapMaterial    _environmentMapMaterial;

        static EMEnvironment*       _instance;

    public:
        static EMEnvironment& GXCALL GetInstance ();
        ~EMEnvironment ();

        GXVoid SetEnvironmentMap ( GXTextureCubeMap& cubeMap );
        GXVoid SetEnvironmentQuasiDistance ( GXFloat meters );

        GXTextureCubeMap& GetEnvironmentMap ();

        GXVoid Render ( GXFloat deltaTime );
        GXVoid OnViewerLocationChanged ();

    private:
        EMEnvironment ();

        EMEnvironment ( const EMEnvironment &other ) = delete;
        EMEnvironment& operator = ( const EMEnvironment &other ) = delete;
};


#endif // EM_ENVIRONMENT
