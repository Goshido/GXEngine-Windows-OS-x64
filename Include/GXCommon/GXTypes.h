//version 1.6

#ifndef GX_TYPES
#define GX_TYPES


#define WIN32_LEAN_AND_MEAN			//to correct include WinSock2.h
#include <Windows.h>

typedef signed __int16			GXShort;
typedef unsigned __int16		GXUShort;
typedef signed __int32			GXInt;
typedef unsigned __int32		GXUInt;
typedef signed long				GXLong;
typedef unsigned long			GXULong;
typedef long long				GXLongLong;
typedef unsigned long long		GXULongLong;
typedef unsigned long			GXDword;
typedef signed __int8			GXByte;
typedef unsigned __int8			GXUByte;
typedef char					GXChar;
typedef unsigned char			GXUChar;
typedef char					GXUTF8;
typedef char					GXMBChar;
typedef wchar_t					GXWChar;
typedef float					GXFloat;
typedef double					GXDouble;
typedef bool					GXBool;
typedef void					GXVoid;
typedef ptrdiff_t				GXPointer;
typedef size_t					GXUPointer;

#define	GXCALL			WINAPI
#define GXCDECLCALL		__cdecl
#define GXTHREADCALL	WINAPI

#define GX_TRUE			true
#define GX_FALSE		false


#endif //GX_TYPES