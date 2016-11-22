//version 1.2

#ifndef GX_COMMON
#define GX_COMMON


#include <WinSock2.h>
#include <Windows.h>
#include "GXLogger.h"
#include "GXTypes.h"


#define GX_MAX_NAME_LENGTH		64

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


#endif //GX_COMMON