﻿// version 1.2

#ifndef GX_MUTEX
#define GX_MUTEX


#include "GXTypes.h"


class GXAbstractMutex
{
	public:
		GXAbstractMutex ();
		virtual ~GXAbstractMutex ();

		virtual GXVoid Lock () = 0;
		virtual GXVoid Release () = 0;

	private:
		GXAbstractMutex ( const GXAbstractMutex &other ) = delete;
		GXAbstractMutex& operator = ( const GXAbstractMutex &other ) = delete;
};


#ifdef __GNUC__
	#include "Posix/GXMutex.h"
#else
	#include "Windows/GXMutex.h"
#endif // __GNU__


#endif // GX_MUTEX
