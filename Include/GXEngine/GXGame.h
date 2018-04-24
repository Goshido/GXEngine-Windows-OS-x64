// verison 1.1

#ifndef GX_GAME
#define GX_GAME


#include <GXCommon/GXTypes.h>


class GXGame
{
	public:
		GXGame ();
		virtual ~GXGame ();

		virtual GXVoid OnInit () = 0;
		virtual GXVoid OnResize ( GXInt width, GXInt height ) = 0;
		virtual GXVoid OnFrame ( GXFloat deltaTime ) = 0;
		virtual GXVoid OnDestroy () = 0;

	private:
		GXGame ( const GXGame &other ) = delete;
		GXGame& operator = ( const GXGame &other ) = delete;
};


#endif // GX_GAME
