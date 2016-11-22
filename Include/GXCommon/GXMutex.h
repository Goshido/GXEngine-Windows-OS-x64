//version 1.0

#ifndef GX_MUTEX
#define GX_MUTEX


#include "GXTypes.h"


class GXMutex
{
	private:
		HANDLE	mutex;

	public:
		GXMutex ();
		~GXMutex ();

		GXVoid Lock ();
		GXVoid Release ();
};


#endif //GX_MUTEX