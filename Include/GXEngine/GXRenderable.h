// version 1.2

#ifndef GX_RENDERABLE
#define GX_RENDERABLE


#include <GXCommon/GXTypes.h>


class GXRenderable
{
    public:
        virtual ~GXRenderable ();

        virtual GXVoid Render () = 0;

    protected:
        GXRenderable ();

        virtual GXVoid InitGraphicResources () = 0;
};


#endif // GX_RENDERABLE
