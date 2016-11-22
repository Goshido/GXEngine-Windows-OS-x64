//version 1.1

#include <GXEngine/GXSoundChannel.h>


GXSoundChannel::GXSoundChannel ()
{
	top = 0;
	next = prev = 0;
	volume = 1.0f;
	emitters = 0;
}

GXSoundChannel::~GXSoundChannel ()
{
	if ( top )
	{
		if ( next ) next->prev = prev;
		if ( prev ) prev->next = next;
		else *top = next;
	}

	GXSoundEmitter* e = emitters;
	while ( e )
	{
		GXSoundEmitter* nextEmitter = e->next;
		delete e;
		e = nextEmitter;
	}
}

GXVoid GXSoundChannel::SetVolume ( GXFloat volume )
{
	this->volume = volume;

	for ( GXSoundEmitter* e = emitters; e; e = e->next )
		e->SetChannelVolume ( this->volume );
}

GXVoid GXSoundChannel::AddEmitter ( GXSoundEmitter* emitter )
{
	if ( !emitter ) GXDebugBox ( L"GXSoundChannel::AddEmitter::Error - добавление звукового эмиттера по нулевому указателю!" );

	emitter->prev = 0;

	emitter->next = emitters;
	if ( emitter->next ) emitter->next->prev = emitter;
	emitters = emitter;

	emitter->top = &emitters;
}

GXVoid GXSoundChannel::Update ()
{
	for ( GXSoundEmitter* e = emitters; e; e = e->next )
		e->Update ();
}