#ifndef EM_DIRECTED_LIGHT_ACTOR
#define EM_DIRECTED_LIGHT_ACTOR


#include "EMActor.h"
#include "EMLight.h"


class EMDirectedLightActor : public EMActor
{
	private:
		EMDirectedLight*	light;

	public:
		EMDirectedLightActor ( const GXWChar* name, const GXTransform &transform );
		~EMDirectedLightActor () override;
		
		GXVoid OnSave ( GXUByte** data ) override;
		GXVoid OnLoad ( const GXUByte* data ) override;
		GXUInt OnRequeredSaveSize () override;
};


#endif // EM_DIRECTED_LIGHT_ACTOR
