#include "EMActor.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXMeshGeometry.h>


class EMMeshActor : public EMActor
{
	private:
		GXMeshGeometry						mesh;
		GXTexture2D							albedo;
		GXTexture2D							normal;
		GXTexture2D							emission;
		GXTexture2D							parameter;
		EMCookTorranceCommonPassMaterial	material;

	public:
		explicit EMMeshActor ( const GXWChar* name, const GXTransform& transform );
		~EMMeshActor () override;

		GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;

		GXVoid SetMesh ( const GXWChar* meshFile );
		EMCookTorranceCommonPassMaterial& GetMaterial ();

	private:
		EMMeshActor () = delete;
		EMMeshActor ( const EMMeshActor &other ) = delete;
		EMMeshActor& operator = ( const EMMeshActor &other ) = delete;
};
