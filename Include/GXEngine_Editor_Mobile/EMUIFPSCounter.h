#ifndef EM_UI_FPS_COUNTER
#define EM_UI_FPS_COUNTER


#include <GXEngine/GXHudSurface.h>


class EMUIFPSCounter
{
	private:
		GXFont					font;
		GXHudSurface*			surface;
		GXUInt					lastFPS;
		GXWChar					fpsBuffer[ 16 ];

		static EMUIFPSCounter*	instance;

	public:
		static EMUIFPSCounter& GetInstance ();
		~EMUIFPSCounter ();

		void Render ();

	private:
		EMUIFPSCounter ();
};


#endif // EM_UI_FPS_COUNTER
