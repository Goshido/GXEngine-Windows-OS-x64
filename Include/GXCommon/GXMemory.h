// version 1.4

#ifndef GX_MEMORY
#define GX_MEMORY


#include <GXCommon/GXTypes.h>

GX_DISABLE_COMMON_WARNINGS

#include <cstdlib>
#include <cstring>

GX_RESTORE_WARNING_STATE


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

class GXCircleBuffer final
{
	private:
		GXUPointer		size;
		GXUPointer		offset;
		GXUByte*		buffer;

	public:
		explicit GXCircleBuffer ( GXUPointer size );
		~GXCircleBuffer ();

		GXVoid* Allocate ( GXUPointer bytes );

	private:
		GXCircleBuffer ( const GXCircleBuffer &other ) = delete;
		GXCircleBuffer& operator = ( const GXCircleBuffer &other ) = delete;
};

class GXDynamicArray final
{
	protected:
		GXUByte*	data;
		GXUPointer	elementSize;
		GXUPointer	numElements;

	public:
		explicit GXDynamicArray ( GXUPointer elementSize );
		~GXDynamicArray ();

		GXVoid SetValue ( GXUPointer i, const GXVoid* element );
		GXVoid* GetValue ( GXUPointer i ) const;
		GXVoid* GetData () const;
		GXUPointer GetLength () const;

		GXVoid Resize ( GXUPointer totalElements );

	private:
		GXDynamicArray ( const GXDynamicArray &other ) = delete;
		GXDynamicArray& operator = ( const GXDynamicArray &other ) = delete;
};


#endif // GX_MEMORY
