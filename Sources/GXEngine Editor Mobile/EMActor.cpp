#include <GXEngine_Editor_Mobile/EMActor.h>
#include <GXCommon/GXStrings.h>


EMActor* em_Actors = 0;


EMActor::EMActor ( const GXWChar* name, GXUInt type, const GXMat4 &transform )
{
	GXWcsclone ( &this->name, name );
	this->type = type;
	memcpy ( &this->transform, &transform, sizeof ( GXMat4 ) );

	isVisible = GX_TRUE;

	prev = 0;
	next = em_Actors;
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
	//NOTHING
}

GXVoid EMActor::OnDrawHudColorPass ()
{
	//NOTHING
}

GXVoid EMActor::OnDrawHudMaskPass ()
{
	//NOTHING
}

GXVoid EMActor::OnUpdate ( GXFloat deltaTime )
{
	//NOTHING
}

GXVoid EMActor::OnSave ( GXUByte** data )
{
	//NOTHING
}

GXVoid EMActor::OnLoad ( const GXUByte* data )
{
	//NOTHING
}

GXUInt EMActor::OnRequeredSaveSize ()
{
	//NOTHING
	return 0;
}

GXVoid EMActor::OnTransformChanged ()
{
	//NOTHING
}

const GXWChar* EMActor::GetName ()
{
	return name;
}

GXUInt EMActor::GetType ()
{
	return type;
}

const GXMat4& EMActor::GetTransform ()
{
	return transform;
}

GXVoid EMActor::SetTransform ( const GXMat4 &transfrom )
{
	memcpy ( &this->transform, &transfrom, sizeof ( GXMat4 ) );
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
