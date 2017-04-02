#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"


#define EM_UNIT_ACTOR_CLASS		0


class EMUnitActorMesh;

class EMUnitActor : public EMActor
{
	private:
		EMUnitActorMesh*	mesh;

	public:
		explicit EMUnitActor ( const GXWChar* name, const GXMat4 &transform );
		~EMUnitActor () override;

		GXVoid OnDrawCommonPass () override;

		GXVoid OnSave ( GXUByte** data ) override;
		GXVoid OnLoad ( const GXUByte* data ) override;
		GXUInt OnRequeredSaveSize () override;
		GXVoid OnTransformChanged () override;
};


#endif //EM_UNIT_ACTOR
