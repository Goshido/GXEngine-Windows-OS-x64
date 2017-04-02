#include <GXEngine_Editor_Mobile/EMDirectedLightActor.h>
#include <cstdlib>


struct EMDirectedLightActorSaveData
{
	GXUByte		baseColor[ 3 ];
	GXFloat		baseIntensity;

	GXUByte		ambientBaseColor[ 3 ];
	GXFloat		ambientIntensity;
};

//---------------------------------------------------------------------------

EMDirectedLightActor::EMDirectedLightActor ( const GXWChar* name, const GXMat4 &transform ):
EMActor ( name, EM_DIRECTED_LIGHT_ACTOR_CLASS, transform )
{
	light = new EMDirectedLight ();
	light->SetRotation ( transform );
}

EMDirectedLightActor::~EMDirectedLightActor ()
{
	delete light;
}

GXVoid EMDirectedLightActor::OnDrawHudDepthIndependentPass ()
{
	//TODO
}

GXVoid EMDirectedLightActor::OnSave ( GXUByte** data, GXUInt &size )
{
	size = sizeof ( EMDirectedLightActorSaveData );
	EMDirectedLightActorSaveData* save = (EMDirectedLightActorSaveData*)malloc ( size );
	
	light->GetBaseColor ( save->baseColor[ 0 ], save->baseColor[ 1 ], save->baseColor[ 2 ] );
	save->baseIntensity = light->GetIntensity ();

	light->GetAmbientBaseColor ( save->ambientBaseColor[ 0 ], save->ambientBaseColor[ 1 ], save->ambientBaseColor[ 2 ] );
	save->ambientIntensity = light->GetAmbientIntensity ();

	*data = (GXUByte*)save;
}

GXVoid EMDirectedLightActor::OnLoad ( const GXUByte* data )
{
	EMDirectedLightActorSaveData* lightSave = (EMDirectedLightActorSaveData*)data;

	light->SetBaseColor ( lightSave->baseColor[ 0 ], lightSave->baseColor[ 1 ], lightSave->baseColor[ 2 ] );
	light->SetIntensity ( lightSave->baseIntensity );

	light->SetAmbientBaseColor ( lightSave->ambientBaseColor[ 0 ], lightSave->ambientBaseColor[ 1 ], lightSave->ambientBaseColor[ 2 ] );
	light->SetAmbientIntensity ( lightSave->ambientIntensity );
}

GXUInt EMDirectedLightActor::OnRequeredSaveSize ()
{
	return sizeof ( EMDirectedLightActorSaveData );
}
