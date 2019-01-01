// version 1.6

#include <GXCommon/GXMemory.h>


GXCircleBuffer::GXCircleBuffer ( GXUPointer size ):
    size ( size ),
    offset ( 0u )
{
    buffer = static_cast<GXUByte*> ( malloc ( size ) );
}

GXCircleBuffer::~GXCircleBuffer ()
{
    free ( buffer );
}

GXVoid* GXCircleBuffer::Allocate ( GXUPointer bytes )
{
    if ( bytes == 0 ) return nullptr;

    if ( ( offset + bytes ) >= size )
    {
        offset = bytes;
        return buffer;
    }

    GXUByte* b = buffer + offset;
    offset += bytes;

    return b;
}

//---------------------------------------------------------------------------------------------------------------------

GXDynamicArray::GXDynamicArray ( GXUPointer elementSize ):
    data ( nullptr ),
    elementSize ( elementSize ),
    numElements ( 0u )
{
    // NOTHING
}

GXDynamicArray::~GXDynamicArray ()
{
    GXSafeFree ( data );
}

GXVoid GXDynamicArray::SetValue ( GXUPointer i, const GXVoid* element )
{
    if ( i >= numElements )
        Resize ( i + 1 );

    memcpy ( data + i * elementSize, element, elementSize );
}

GXVoid* GXDynamicArray::GetValue ( GXUPointer i ) const
{
    if ( i >= numElements )
        return nullptr;

    return data + i * elementSize;
}

GXVoid* GXDynamicArray::GetData () const
{
    return data;
}

GXUPointer GXDynamicArray::GetLength () const
{
    return numElements;
}

GXVoid GXDynamicArray::Resize ( GXUPointer totalElements )
{
    GXUByte* old = data;
    data = static_cast<GXUByte*> ( malloc ( totalElements * elementSize ) );

    if ( !old )
    {
        numElements = totalElements;
        return;
    }

    GXUPointer copy = ( totalElements < numElements ) ? totalElements : numElements;
    memcpy ( data, old, copy * elementSize );
    numElements = totalElements;

    GXSafeFree ( old );
}
