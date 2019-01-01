#ifndef BB_GAME
#define BB_GAME


#include "BBLabel.h"
#include <GXEngine/GXCameraOrthographic.h>
#include <GXEngine/GXGame.h>
#include <GXCommon/GXMemory.h>


class BBGame final : public GXMemoryInspector, public GXGame
{
    public:
        GXBool                  needToInitGUI;
        BBLabel*                testLabel;

        GXCameraOrthographic    guiCamera;

    public:
        BBGame ();
        ~BBGame () override;

    private:
        GXVoid OnInit () override;
        GXVoid OnResize ( GXInt width, GXInt height ) override;
        GXVoid OnFrame ( GXFloat deltaTime ) override;
        GXVoid OnDestroy () override;

        GXVoid InitGUI ();
        GXVoid UpdateGUI ();

        BBGame ( const BBGame &other ) = delete;
        BBGame& operator = ( const BBGame &other ) = delete;
};


#endif // BB_GAME
