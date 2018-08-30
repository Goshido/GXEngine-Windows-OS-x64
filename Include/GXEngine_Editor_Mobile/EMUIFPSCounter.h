#ifndef EM_UI_FPS_COUNTER
#define EM_UI_FPS_COUNTER


#include <GXEngine/GXHudSurface.h>


class EMUIFPSCounter final
{
	private:
		GXUInt					lastFPS;
		GXFont					font;

		GXHudSurface*			surface;
		GXWChar					fpsBuffer[ 16 ];

		static EMUIFPSCounter*	instance;

	public:
		static EMUIFPSCounter& GetInstance ();
		~EMUIFPSCounter ();

		void Render ();

	private:
		EMUIFPSCounter ();

		EMUIFPSCounter ( const EMUIFPSCounter &other ) = delete;
		EMUIFPSCounter& operator = ( const EMUIFPSCounter &other ) = delete;
};


#endif // EM_UI_FPS_COUNTER
