//version 1.1

#ifndef GX_MEMORY
#define GX_MEMORY


#include <GXCommon/GXTypes.h>
#include <cstdlib>
#include <cstring>


#define GXSafeDelete(ptr)		\
		if ( ptr )				\
		{						\
			delete ptr;			\
			ptr = nullptr;		\
		}

#define GXSafeFree(ptr)			\
		if ( ptr )				\
		{						\
			free ( ptr );		\
			ptr = nullptr;		\
		}

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
		~GXDynamicArray ();

		GXVoid SetValue ( GXUInt i, const GXVoid* element );
		GXVoid* GetValue ( GXUInt i ) const;
		GXVoid* GetData () const;
		GXUInt GetLength () const;

		GXVoid Resize ( GXUInt numElements );
};


#endif //GX_MEMORY
