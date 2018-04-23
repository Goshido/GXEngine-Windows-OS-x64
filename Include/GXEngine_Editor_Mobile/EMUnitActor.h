#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"
#include "EMMesh.h"
#include "EMCookTorranceCommonPassMaterial.h"
#include <GXEngine/GXTexture2D.h>


class EMUnitActor : public EMActor
{
	private:
		EMMesh								mesh;
		EMCookTorranceCommonPassMaterial	commonPassMaterial;
		GXTexture2D							albedoTexture;
		GXTexture2D							normalTexture;
		GXTexture2D							emissionTexture;
		GXTexture2D							parameterTexture;

	public:
		explicit EMUnitActor ( const GXWChar* name, const GXTransform &transform );
		~EMUnitActor () override;

		GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;

		GXVoid OnSave ( GXUByte** data ) override;
		GXVoid OnLoad ( const GXUByte* data ) override;
		GXUInt OnRequeredSaveSize () override;
		GXVoid OnTransformChanged () override;
};


#endif // EM_UNIT_ACTOR
