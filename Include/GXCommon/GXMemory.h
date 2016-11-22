//version 1.0

#ifndef GX_MEMORY
#define GX_MEMORY


#include <GXCommon/GXTypes.h>


class GXCircleBuffer
{
	private:
		GXUByte*	buffer;
		GXUInt		size;
		GXUInt		offset;

	public:
		GXCircleBuffer ( GXUInt size );
		~GXCircleBuffer ();

		GXVoid* Allocate ( GXUInt size );
};

class GXDynamicArray
{
	protected:
		GXUByte*	data;
		GXUInt		elementSize;
		GXUInt		numElements;

	public:
		GXDynamicArray ( GXUInt elementSize );
		virtual ~GXDynamicArray ();

		GXVoid SetValue ( GXUInt i, const GXVoid* element );
		const GXVoid* GetValue ( GXUInt i );
		const GXVoid* GetData ();
		GXUInt GetLength ();

		GXVoid Resize ( GXUInt numElements );
};


#endif //GX_MEMORY
