#include <GXEngine_Editor_Mobile/EMActor.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


EMActor* em_Actors = nullptr;

EMActor::EMActor ( const GXWChar* name, eEMActorType type, const GXTransform &transform ):
	next ( em_Actors ),
	prev ( nullptr ),
	type ( type ),
	transform ( transform ),
	isVisible ( GX_TRUE )
{
	GXWcsclone ( &this->name, name );

	if ( em_Actors )
		em_Actors->prev = this;

	em_Actors = this;
}

EMActor::~EMActor ()
{
	if ( next ) next->prev = prev;

	if ( prev ) 
		prev->next = next;
	else
		em_Actors = next;

	free ( name );
}

GXVoid EMActor::OnDrawCommonPass ( GXFloat /*deltaTime*/ )
{
	// NOTHING
}

GXVoid EMActor::OnDrawHudColorPass ()
{
	// NOTHING
}

GXVoid EMActor::OnDrawHudMaskPass ()
{
	// NOTHING
}

GXVoid EMActor::OnUpdate ( GXFloat /*deltaTime*/ )
{
	// NOTHING
}

GXVoid EMActor::OnSave ( GXUByte** /*data*/ )
{
	// NOTHING
}

GXVoid EMActor::OnLoad ( const GXUByte* /*data*/ )
{
	// NOTHING
}

GXUPointer EMActor::OnRequeredSaveSize () const
{
	// NOTHING
	return 0u;
}

GXVoid EMActor::OnTransformChanged ()
{
	// NOTHING
}

const GXWChar* EMActor::GetName ()
{
	return name;
}

eEMActorType EMActor::GetType ()
{
	return type;
}

const GXTransform& EMActor::GetTransform ()
{
	return transform;
}

GXVoid EMActor::SetTransform ( const GXTransform &newTransform )
{
	transform = newTransform;
	OnTransformChanged ();
}

GXVoid EMActor::Show ()
{
	isVisible = GX_TRUE;
}

GXVoid EMActor::Hide ()
{
	isVisible = GX_FALSE;
}
