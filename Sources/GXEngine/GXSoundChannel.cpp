// version 1.5

#include <GXEngine/GXSoundChannel.h>


#define DEFAULT_CHANNEL_VOLUME      1.0f

//--------------------------------------------------------------------------

GXSoundChannel::GXSoundChannel ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSoundChannel" )
    top ( nullptr ),
    next ( nullptr ),
    prev ( nullptr ),
    volume ( DEFAULT_CHANNEL_VOLUME ),
    emitters ( nullptr )
{
    // NOTHING
}

GXSoundChannel::~GXSoundChannel ()
{
    if ( top )
    {
        if ( next )
            next->prev = prev;

        if ( prev )
        {
            prev->next = next;
        }
        else
        {
            *top = next;
        }
    }

    GXSoundEmitter* e = emitters;

    while ( e )
    {
        GXSoundEmitter* nextEmitter = e->next;
        delete e;
        e = nextEmitter;
    }
}

GXVoid GXSoundChannel::SetVolume ( GXFloat volumeLevel )
{
    volume = volumeLevel;

    for ( GXSoundEmitter* e = emitters; e; e = e->next )
    {
        e->SetChannelVolume ( volume );
    }
}

GXVoid GXSoundChannel::AddEmitter ( GXSoundEmitter* emitter )
{
    if ( !emitter )
        GXWarningBox ( L"GXSoundChannel::AddEmitter::Error - добавление звукового эмиттера по нулевому указателю!" );

    emitter->prev = nullptr;

    emitter->next = emitters;

    if ( emitter->next )
        emitter->next->prev = emitter;

    emitters = emitter;
    emitter->top = &emitters;
}

GXVoid GXSoundChannel::Update ()
{
    for ( GXSoundEmitter* e = emitters; e; e = e->next )
    {
        e->Update ();
    }
}
