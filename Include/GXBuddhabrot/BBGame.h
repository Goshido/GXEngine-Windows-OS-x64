#ifndef BB_GAME
#define BB_GAME


#include <GXEngine/GXGame.h>


class BBGame final : public GXGame
{
    public:
        BBGame ();
        ~BBGame () override;

    private:
        GXVoid OnInit () override;
        GXVoid OnResize ( GXInt width, GXInt height ) override;
        GXVoid OnFrame ( GXFloat deltaTime ) override;
        GXVoid OnDestroy () override;

        BBGame ( const BBGame &other ) = delete;
        BBGame& operator = ( const BBGame &other ) = delete;
};


#endif // BB_GAME
