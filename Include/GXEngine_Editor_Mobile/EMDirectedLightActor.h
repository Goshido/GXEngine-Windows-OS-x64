#ifndef EM_DIRECTED_LIGHT_ACTOR
#define EM_DIRECTED_LIGHT_ACTOR


#include "EMActor.h"
#include "EMLight.h"


#define EM_DIRECTED_LIGHT_ACTOR_CLASS		1


class EMDirectedLightActor : EMActor
{
	private:
		EMDirectedLight*	light;

	public:
		EMDirectedLightActor ( const GXWChar* name, const GXMat4 &transform );
		virtual ~EMDirectedLightActor ();

		virtual GXVoid OnDrawHudDepthIndependentPass ();
		
		virtual GXVoid OnSave ( GXUByte** data, GXUInt &size );
		virtual GXVoid OnLoad ( const GXUByte* data );
		virtual GXUInt OnRequeredSaveSize ();
};


#endif //EM_DIRECTED_LIGHT_ACTOR
