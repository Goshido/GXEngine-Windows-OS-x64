#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"
#include <GXEngine/GXMesh.h>


#define EM_UNIT_ACTOR_CLASS		0


class EMUnitActorMesh;

class EMUnitActor : public EMActor
{
	private:
		EMUnitActorMesh*	mesh;

	public:
		EMUnitActor ( const GXWChar* name, const GXMat4 &transform );
		virtual ~EMUnitActor ();

		virtual GXVoid OnDrawCommonPass ();

		virtual GXVoid OnSave ( GXUByte** data );
		virtual GXVoid OnLoad ( const GXUByte* data );
		virtual GXUInt OnRequeredSaveSize ();
		virtual GXVoid OnTransformChanged ();
};


#endif //EM_UNIT_ACTOR
