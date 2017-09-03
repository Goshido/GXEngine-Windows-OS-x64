//version 1.1

#include <GXCommon/GXMemory.h>


GXCircleBuffer::GXCircleBuffer ( GXUInt size )
{
	this->size = size;
	offset = 0;
	buffer = (GXUByte*)malloc ( size );
}

GXCircleBuffer::~GXCircleBuffer ()
{
	free ( buffer );
}

GXVoid* GXCircleBuffer::Allocate ( GXUInt bytes )
{
	if ( bytes == 0 ) return 0;

	if ( ( offset + bytes ) >= size )
	{
		offset = bytes;
		return buffer;
	}

	GXUByte* b = buffer + offset;
	offset += bytes;

	return b;
}

//------------------------------------------------------------------------------------------------

GXDynamicArray::GXDynamicArray ( GXUInt elementSize )
{
	this->elementSize =  elementSize;
	data = 0;
	numElements = 0;
}

GXDynamicArray::~GXDynamicArray ()
{
	GXSafeFree ( data );
}

GXVoid GXDynamicArray::SetValue ( GXUInt i, const GXVoid* element )
{
	if ( i >= numElements )
		Resize ( i + 1 );

	memcpy ( data + i * elementSize, element, elementSize );
}

GXVoid* GXDynamicArray::GetValue ( GXUInt i ) const
{
	if ( i >= numElements ) return 0;

	return data + i * elementSize;
}

GXVoid* GXDynamicArray::GetData () const
{
	return data;
}

GXUInt GXDynamicArray::GetLength () const
{
	return numElements;
}

GXVoid GXDynamicArray::Resize ( GXUInt totalElements )
{
	GXUByte* old = data;
	data = (GXUByte*)malloc ( totalElements * elementSize );

	if ( !old )
	{
		numElements = totalElements;
		return;
	}

	GXUInt copy = ( totalElements < numElements ) ? totalElements : numElements;
	memcpy ( data, old, copy * elementSize );
	numElements = totalElements;

	GXSafeFree ( old );
}
