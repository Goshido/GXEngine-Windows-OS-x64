// version 1.2

#ifndef GX_MATERIAL
#define GX_MATERIAL


#include "GXShaderProgram.h"
#include "GXTransform.h"


class GXMaterial
{
    protected:
        GXShaderProgram     shaderProgram;

    public:
        GXMaterial ();
        virtual ~GXMaterial ();

        virtual GXVoid Bind ( const GXTransform &transform ) = 0;
        virtual GXVoid Unbind () = 0;

    private:
        GXMaterial ( const GXMaterial &other ) = delete;
        GXMaterial& operator = ( const GXMaterial &other ) = delete;
};


#endif // GX_MATERIAL
