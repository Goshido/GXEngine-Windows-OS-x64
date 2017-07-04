//version 1.0

#ifndef GX_MUTEX_WINDOWS
#define GX_MUTEX_WINDOWS


#include "../GXTypes.h"


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


#endif //GX_MUTEX_WINDOWS
