// version 1.4

#ifndef GX_THREAD_WINDOWS
#define GX_THREAD_WINDOWS


#include <GXCommon/GXThread.h>


class GXThread final : public GXAbstractThread
{
	private:
		HANDLE				thread;

	public:
		explicit GXThread ( PFNGXTHREADPROC procedure, GXVoid* argument );
		~GXThread () override;

		GXVoid Start () override;
		GXVoid Switch () override;
		GXVoid Sleep ( GXUInt milliseconds ) override;
		GXVoid Join () override;

	private:
		static unsigned __stdcall RootThreadStarter ( void* param );

		GXThread () = delete;
		GXThread ( const GXThread &other ) = delete;
		GXThread& operator = ( const GXThread &other ) = delete;
};


#endif // GX_THREAD_WINDOWS
