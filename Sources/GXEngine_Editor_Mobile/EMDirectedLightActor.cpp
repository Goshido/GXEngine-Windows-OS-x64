#include <GXEngine_Editor_Mobile/EMDirectedLightActor.h>

#define DEFAULT_LIGHT_INTENSITY     5.0f

//---------------------------------------------------------------------------------------------------------------------

EMDirectedLightActor::EMDirectedLightActor ( const GXWChar* name, const GXTransform &transform ):
    EMActor ( name, eEMActorType::DirectedLight, transform ),
    _light ( new EMDirectedLight () )
{
    _light->SetRotation ( transform.GetRotation () );
    _light->SetIntensity ( DEFAULT_LIGHT_INTENSITY );
}

EMDirectedLightActor::~EMDirectedLightActor ()
{
    delete _light;
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
