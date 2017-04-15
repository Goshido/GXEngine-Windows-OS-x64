#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"
#include "EMMesh.h"
#include "EMBlinnPhongCommonPassMaterial.h"
#include <GXEngine/GXTexture.h>


#define EM_UNIT_ACTOR_CLASS		0


class EMUnitActor : public EMActor
{
	private:
		EMMesh							mesh;
		EMBlinnPhongCommonPassMaterial	commonPassMaterial;
		GXTexture						diffuseTexture;
		GXTexture						normalTexture;
		GXTexture						specularTexture;
		GXTexture						emissionTexture;

	public:
		explicit EMUnitActor ( const GXWChar* name, const GXMat4 &transform );
		~EMUnitActor () override;

		GXVoid OnDrawCommonPass ( GXFloat deltaTime ) override;

		GXVoid OnSave ( GXUByte** data ) override;
		GXVoid OnLoad ( const GXUByte* data ) override;
		GXUInt OnRequeredSaveSize () override;
		GXVoid OnTransformChanged () override;
};


#endif //EM_UNIT_ACTOR
