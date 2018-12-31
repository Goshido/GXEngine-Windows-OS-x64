// version 1.1

#ifndef GX_RENDERABLE
#define GX_RENDERABLE


#include <GXCommon/GXTypes.h>


class GXRenderable
{
    public:
        GXRenderable ();
        virtual ~GXRenderable ();

        virtual GXVoid Render () = 0;

    protected:
        virtual GXVoid InitGraphicResources () = 0;
};


#endif // GX_RENDERABLE
