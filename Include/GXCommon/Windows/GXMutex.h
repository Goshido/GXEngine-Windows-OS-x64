//version 1.1

#ifndef GX_MUTEX_WINDOWS
#define GX_MUTEX_WINDOWS


#include <GXCommon/GXMutex.h>


class GXMutex : public GXAbstractMutex
{
	private:
		HANDLE	mutex;

	public:
		GXMutex ();
		~GXMutex () override;

		GXVoid Lock () override;
		GXVoid Release () override;
};


#endif //GX_MUTEX_WINDOWS
