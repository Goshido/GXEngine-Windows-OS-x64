// version 1.9

#include <GXCommon/GXMemory.h>


GXCircleBuffer::GXCircleBuffer ( GXUPointer size )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXCircleBuffer" )
    _size ( size ),
    _offset ( 0u )
{
    _buffer = static_cast<GXUByte*> ( Malloc ( size ) );
}

GXCircleBuffer::~GXCircleBuffer ()
{
    Free ( _buffer );
}

GXVoid* GXCircleBuffer::Allocate ( GXUPointer bytes )
{
    if ( bytes == 0 ) return nullptr;

    if ( ( _offset + bytes ) >= _size )
    {
        _offset = bytes;
        return _buffer;
    }

    GXUByte* b = _buffer + _offset;
    _offset += bytes;

    return b;
}

//---------------------------------------------------------------------------------------------------------------------

GXDynamicArray::GXDynamicArray ( GXUPointer elementSize ):
    _data ( nullptr ),
    _elementSize ( elementSize ),
    _numElements ( 0u )
{
    // NOTHING
}

GXDynamicArray::~GXDynamicArray ()
{
    GXSafeFree ( _data );
}

GXVoid GXDynamicArray::SetValue ( GXUPointer i, const GXVoid* element )
{
    if ( i >= _numElements )
        Resize ( i + 1 );

    memcpy ( _data + i * _elementSize, element, _elementSize );
}

GXVoid* GXDynamicArray::GetValue ( GXUPointer i ) const
{
    if ( i >= _numElements )
        return nullptr;

    return _data + i * _elementSize;
}

GXVoid* GXDynamicArray::GetData () const
{
    return _data;
}

GXUPointer GXDynamicArray::GetLength () const
{
    return _numElements;
}

GXVoid GXDynamicArray::Resize ( GXUPointer totalElements )
{
    // TODO improve heap memory allocation
    // TODO use memory inspector approach

    GXUByte* old = _data;
    _data = static_cast<GXUByte*> ( malloc ( totalElements * _elementSize ) );

    if ( !old )
    {
        _numElements = totalElements;
        return;
    }

    GXUPointer copy = ( totalElements < _numElements ) ? totalElements : _numElements;
    memcpy ( _data, old, copy * _elementSize );
    _numElements = totalElements;

    GXSafeFree ( old );
}
