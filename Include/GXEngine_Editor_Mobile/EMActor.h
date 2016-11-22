#ifndef EM_ACTOR
#define EM_ACTOR


#include <GXCommon/GXMath.h>


class EMActor;
extern EMActor* em_Actors;


struct EMActorHeader
{
	GXUInt		type;
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
		GXUInt			type;
		GXMat4			transform;
		GXBool			isVisible;

	public:
		EMActor ( const GXWChar* name, GXUInt type, const GXMat4 &transform );
		virtual ~EMActor ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();
		
		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnSave ( GXUByte** data );
		virtual GXVoid OnLoad ( const GXUByte* data );
		virtual GXUInt OnRequeredSaveSize ();
		virtual GXVoid OnTransformChanged ();

		const GXWChar* GetName ();
		GXUInt GetType ();

		const GXMat4& GetTransform ();
		GXVoid SetTransform ( const GXMat4 &transform );

		GXVoid Show ();
		GXVoid Hide ();
};


#endif //EM_ACTOR
