// version 1.3

#ifndef GX_THREAD_WINDOWS
#define GX_THREAD_WINDOWS


#include <GXCommon/GXThread.h>


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

		GXThread ( const GXThread &other ) = delete;
		GXThread& operator = ( const GXThread &other ) = delete;
};


#endif // GX_THREAD_WINDOWS
