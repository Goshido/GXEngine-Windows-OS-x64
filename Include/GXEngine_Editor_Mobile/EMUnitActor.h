#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"
#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXTexture2D.h>


#define EM_UNIT_ACTOR_CLASS		0


class EMUnitActor : public EMActor
{
	private:
		EMMesh								mesh;
		EMCookTorranceCommonPassMaterial	commonPassMaterial;
		GXTexture2D							diffuseTexture;
		GXTexture2D							normalTexture;
		GXTexture2D							emissionTexture;
		GXTexture2D							parameterTexture;

	public:
		explicit EMUnitActor ( const GXWChar* name, const GXMat4 &transform );
		~EMUnitActor () override;

		GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;

		GXVoid OnSave ( GXUByte** data ) override;
		GXVoid OnLoad ( const GXUByte* data ) override;
		GXUInt OnRequeredSaveSize () override;
		GXVoid OnTransformChanged () override;

		EMCookTorranceCommonPassMaterial& GetMaterial ();
};


#endif //EM_UNIT_ACTOR
