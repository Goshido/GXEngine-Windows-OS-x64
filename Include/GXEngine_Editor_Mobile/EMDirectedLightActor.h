#ifndef EM_DIRECTED_LIGHT_ACTOR
#define EM_DIRECTED_LIGHT_ACTOR


#include "EMActor.h"
#include "EMLight.h"


class EMDirectedLightActor final : public EMActor
{
	private:
		EMDirectedLight*	light;

	public:
		explicit EMDirectedLightActor ( const GXWChar* name, const GXTransform &transform );
		~EMDirectedLightActor () override;
		
		GXVoid OnSave ( GXUByte** data ) override;
		GXVoid OnLoad ( const GXUByte* data ) override;
		GXUPointer OnRequeredSaveSize () const override;

	private:
		EMDirectedLightActor () = delete;
		EMDirectedLightActor ( const EMDirectedLightActor &other ) = delete;
		EMDirectedLightActor& operator = ( const EMDirectedLightActor &other ) = delete;
};


#endif // EM_DIRECTED_LIGHT_ACTOR
