// version 1.6

#include <GXEngine/GXSoundChannel.h>


#define DEFAULT_CHANNEL_VOLUME      1.0f

//--------------------------------------------------------------------------

GXSoundChannel::GXSoundChannel ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSoundChannel" )
    _top ( nullptr ),
    _next ( nullptr ),
    _previous ( nullptr ),
    _volume ( DEFAULT_CHANNEL_VOLUME ),
    _emitters ( nullptr )
{
    // NOTHING
}

GXSoundChannel::~GXSoundChannel ()
{
    if ( _top )
    {
        if ( _next )
            _next->_previous = _previous;

        if ( _previous )
        {
            _previous->_next = _next;
        }
        else
        {
            *_top = _next;
        }
    }

    GXSoundEmitter* e = _emitters;

    while ( e )
    {
        GXSoundEmitter* nextEmitter = e->_next;
        delete e;
        e = nextEmitter;
    }
}

GXVoid GXSoundChannel::SetVolume ( GXFloat volumeLevel )
{
    _volume = volumeLevel;

    for ( GXSoundEmitter* e = _emitters; e; e = e->_next )
    {
        e->SetChannelVolume ( _volume );
    }
}

GXVoid GXSoundChannel::AddEmitter ( GXSoundEmitter* emitter )
{
    if ( !emitter )
        GXWarningBox ( L"GXSoundChannel::AddEmitter::Error - добавление звукового эмиттера по нулевому указателю!" );

    emitter->_previous = nullptr;

    emitter->_next = _emitters;

    if ( emitter->_next )
        emitter->_next->_previous = emitter;

    _emitters = emitter;
    emitter->_top = &_emitters;
}

GXVoid GXSoundChannel::Update ()
{
    for ( GXSoundEmitter* e = _emitters; e; e = e->_next )
    {
        e->Update ();
    }
}
