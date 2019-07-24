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

GXDynamicArray::GXDynamicArray ( GXUPointer elementSize )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXDynamicArray" )
    _elementSize ( elementSize ),
    _elements ( 0u ),
    _reservedElements ( 0u ),
    _data ( nullptr )
{
    // NOTHING
}

GXDynamicArray::GXDynamicArray ( GXUPointer elementSize, GXUPointer reservedElements )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXDynamicArray" )
    _elementSize ( elementSize ),
    _elements ( 0u ),
    _reservedElements ( reservedElements )
{
    _data = static_cast<GXUByte*> ( Malloc ( _reservedElements * _elementSize ) );
}

GXDynamicArray::~GXDynamicArray ()
{
    SafeFree ( reinterpret_cast<GXVoid**> ( &_data ) );
}

GXVoid GXDynamicArray::Reserve ( GXUPointer elements )
{
    if ( elements <= _reservedElements ) return;

    if ( _reservedElements == 0u )
    {
        _reservedElements = elements;
        _data = static_cast<GXUByte*> ( Malloc ( elements * _elementSize ) );
        return;
    }

    _reservedElements = elements;
    GXUByte* old = _data;
    _data = static_cast<GXUByte*> ( Malloc ( elements * _elementSize ) );
    PreserveElements ( _data, old, _elements );
    Free ( old );
}

GXVoid GXDynamicArray::PushBack ( const GXVoid* element )
{
    SetValue ( _elements, element );
}

GXVoid GXDynamicArray::SetValue ( GXUPointer i, const GXVoid* element )
{
    if ( i < _reservedElements )
    {
        if ( i >= _elements )
            _elements = i + 1u;

        memcpy ( _data + i * _elementSize, element, _elementSize );
        return;
    }

    // Power of two strategy.

    GXUPointer newCapacity = _reservedElements == 0u ? 1u : _reservedElements;

    for ( ; newCapacity <= i; newCapacity *= 2u );

    Reserve ( newCapacity );
    _elements = i + 1u;

    memcpy ( _data + i * _elementSize, element, _elementSize );
}

GXVoid* GXDynamicArray::GetValue ( GXUPointer i ) const
{
    if ( i >= _elements )
        return nullptr;

    return _data + i * _elementSize;
}

GXVoid* GXDynamicArray::GetData () const
{
    return _data;
}

GXUPointer GXDynamicArray::GetLength () const
{
    return _elements;
}

GXVoid GXDynamicArray::Resize ( GXUPointer elements )
{
    GXUByte* old = _data;
    _data = static_cast<GXUByte*> ( Malloc ( elements * _elementSize ) );

    if ( !old )
    {
        _elements = _reservedElements = elements;
        return;
    }

    PreserveElements ( _data, old, elements );
    _elements = _reservedElements = elements;

    Free ( old );
}

GXVoid GXDynamicArray::PreserveElements ( GXUByte* destination, const GXUByte* source, GXUPointer targetElements )
{
    const GXUPointer copy = ( targetElements < _elements ) ? targetElements : _elements;
    memcpy ( destination, source, copy * _elementSize );
}
