#include <GXEngine_Editor_Mobile/EMDirectedLightActor.h>

GX_DISABLE_COMMON_WARNINGS

#include <cstdlib>

GX_RESTORE_WARNING_STATE


#define DEFAULT_LIGHT_INTENSITY		5.0f

//---------------------------------------------------------------------------------------------------------------------

EMDirectedLightActor::EMDirectedLightActor ( const GXWChar* name, const GXTransform &transform ):
	EMActor ( name, eEMActorType::DirectedLight, transform ),
	light ( new EMDirectedLight () )
{
	light->SetRotation ( transform.GetRotation () );
	light->SetIntensity ( DEFAULT_LIGHT_INTENSITY );
}

EMDirectedLightActor::~EMDirectedLightActor ()
{
	delete light;
}

GXVoid EMDirectedLightActor::OnSave ( GXUByte** /*data*/ )
{
	// TODO
}

GXVoid EMDirectedLightActor::OnLoad ( const GXUByte* /*data*/ )
{
	// TODO
}

GXUPointer EMDirectedLightActor::OnRequeredSaveSize () const
{
	// TODO
	return 0u;
}
