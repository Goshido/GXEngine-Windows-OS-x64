#ifndef EM_UI_FPS_COUNTER
#define EM_UI_FPS_COUNTER


#include <GXEngine/GXHudSurface.h>


class EMUIFPSCounter
{
	private:
		GXFont			font;
		GXHudSurface*	surface;
		GXUInt			lastFPS;
		GXWChar			fpsBuffer[ 16 ];

	public:
		EMUIFPSCounter ();
		~EMUIFPSCounter ();

		void Render ();
};


#endif //EM_UI_FPS_COUNTER
