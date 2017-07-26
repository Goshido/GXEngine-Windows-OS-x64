//version 1.2

#ifndef GX_THREAD_WINDOWS
#define GX_THREAD_WINDOWS


#include "../GXThread.h"


class GXThread : public GXAbstractThread
{
	private:
		HANDLE				thread;

	public:
		explicit GXThread ( PFNGXTHREADPROC procedure, GXVoid* argument );
		~GXThread () override;

		GXVoid Start () override;
		GXVoid Switch () override;
		GXVoid Join () override;

	private:
		static DWORD WINAPI RootThreadStarter ( LPVOID lpThreadParameter );
};


#endif //GX_THREAD_WINDOWS
