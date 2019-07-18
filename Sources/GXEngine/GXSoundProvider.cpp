// version 1.8

#include <GXEngine/GXSoundProvider.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


GXSoundTrack* gx_strgSoundTracks = nullptr;

GXSoundStreamer::GXSoundStreamer ( GXVoid* memoryMappedFile, GXUPointer fileSize )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSoundStreamer" )
    _mappedFile ( static_cast<GXUByte*> ( memoryMappedFile ) ),
    _totalSize ( fileSize ),
    _position ( 0 )
{
    // NOTHING
}

GXSoundStreamer::~GXSoundStreamer ()
{
    // NOTHING
}

GXUInt GXSoundStreamer::Read ( GXVoid* out, GXUInt size )
{
    const GXUInt temp = static_cast<GXUInt> ( _totalSize - _position );

    if ( size > temp )
        size = temp;
    
    memcpy ( out, _mappedFile + _position, size );
    _position += size;

    return size;
}

GXInt GXSoundStreamer::Seek ( GXInt offset, GXInt whence )
{
    switch ( whence )
    {
        case SEEK_SET:
            _position = offset;
        break;

        case SEEK_CUR:
            _position += offset;
        break;

        case SEEK_END:
            _position = static_cast<GXLong> ( _totalSize ) + offset;
        break;
    }

    if ( _position < 0 ) 
        _position = 0;
    else if ( _position > static_cast<GXLong> ( _totalSize ) ) 
        _position = static_cast<GXLong> ( _totalSize );

    return 0;
}

GXLong GXSoundStreamer::Tell ()
{
    return _position;
}

GXVoid GXSoundStreamer::Reset ()
{
    _position = 0;
}

//-----------------------------------------------------------------------------------------------------

GXSoundTrack::GXSoundTrack ( const GXWChar* trackFileName )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSoundTrack" )
    _next ( gx_strgSoundTracks ),
    _previous ( nullptr ),
    _references ( 1u ),
    _readyBuffer ( 0u ),
    _trackFile ( trackFileName )
{
    if ( _next )
        _next->_previous = this;

    gx_strgSoundTracks = this;

    GXUPointer fileSize;
    GXFile file ( _trackFile );
    file.LoadContent ( _mappedFile, fileSize, eGXFileContentOwner::User, GX_TRUE );
}

GXVoid GXSoundTrack::AddReference ()
{
    ++_references;
}

GXVoid GXSoundTrack::Release ()
{
    if ( _references < static_cast<GXUPointer> ( 1u ) )
    {
        GXWarningBox ( "GXSoundTrack::Error - ѕопытка уменьшить количество ссылок, когда их нет." );
        return;
    }

    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXSoundTrack::~GXSoundTrack ()
{
    SafeFree ( reinterpret_cast<GXVoid**> ( &_mappedFile ) );

    if ( _next )
        _next->_previous = _previous;

    if ( _previous )
    {
        _previous->_next = _next;
        return;
    }

    gx_strgSoundTracks = _next;
}
