//version 1.0

#include <GXCommon/GXMemory.h>
#include <GXCommon/GXCommon.h>


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

GXVoid* GXCircleBuffer::Allocate ( GXUInt size )
{
	if ( size == 0 ) return 0;

	if ( ( offset + size ) >= this->size )
	{
		offset = size;
		return buffer;
	}

	GXUByte* b = buffer + offset;
	offset += size;

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

const GXVoid* GXDynamicArray::GetValue ( GXUInt i ) const
{
	if ( i >= numElements ) return 0;

	return data + i * elementSize;
}

const GXVoid* GXDynamicArray::GetData () const
{
	return data;
}

GXUInt GXDynamicArray::GetLength () const
{
	return numElements;
}

GXVoid GXDynamicArray::Resize ( GXUInt numElements )
{
	GXUByte* old = data;
	data = (GXUByte*)malloc ( numElements * elementSize );

	if ( !old )
	{
		this->numElements = numElements;
		return;
	}

	GXUInt copy = ( numElements < this->numElements ) ? numElements : this->numElements;
	memcpy ( data, old, copy * elementSize );
	this->numElements = numElements;

	GXSafeFree ( old );
}
