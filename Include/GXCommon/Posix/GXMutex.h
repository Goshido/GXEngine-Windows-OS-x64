//version 1.0

#ifndef GX_MUTEX_POSIX
#define GX_MUTEX_POSIX


#include "../GXTypes.h"
#include <pthread.h>


class GXMutex
{
	private:
		pthread_mutex_t		mutex;

	public:
		GXMutex ();
		~GXMutex ();

		GXVoid Lock ();
		GXVoid Release ();
};


#endif //GX_MUTEX_POSIX
