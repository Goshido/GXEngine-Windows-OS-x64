#ifndef EM_ACTOR
#define EM_ACTOR


#include <GXEngine/GXTransform.h>
#include <GXCommon/GXMath.h>


class EMActor;
extern EMActor* em_Actors;


enum class eEMActorType : GXUBigInt
{
	Abstact = 0,
	DirectedLight = 1,
	UnitCube = 2,
	Mesh = 3,
	PhysicsDrivenActor = 4
};


struct EMActorHeader
{
	GXUBigInt	type;
	GXUInt		nameOffset;
	GXMat4		origin;
	GXBool		isVisible;
	GXUInt		size;
};

class EMActor
{
	private:
		EMActor*		next;
		EMActor*		prev;

	protected:
		GXWChar*		name;
		eEMActorType	type;
		GXTransform		transform;
		GXBool			isVisible;

	public:
		explicit EMActor ( const GXWChar* name, eEMActorType type, const GXTransform &transform );
		virtual ~EMActor ();

		virtual GXVoid OnDrawCommonPass ( GXFloat deltaTime );
		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();
		
		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnSave ( GXUByte** data );
		virtual GXVoid OnLoad ( const GXUByte* data );
		virtual GXUInt OnRequeredSaveSize ();
		virtual GXVoid OnTransformChanged ();

		const GXWChar* GetName ();
		eEMActorType GetType ();

		const GXTransform& GetTransform ();
		GXVoid SetTransform ( const GXTransform &transform );

		GXVoid Show ();
		GXVoid Hide ();
};


#endif //EM_ACTOR
