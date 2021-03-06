#ifndef EM_VERTEX_COLOR_MATERIAL
#define EM_VERTEX_COLOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMVertexColorMaterial final : public GXMaterial
{
    private:
        GLint       _currentFrameModelViewProjectionMatrixLocation;

    public:
        EMVertexColorMaterial ();
        ~EMVertexColorMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

    private:
        EMVertexColorMaterial ( const EMVertexColorMaterial &other ) = delete;
        EMVertexColorMaterial& operator = ( const EMVertexColorMaterial &other ) = delete;
};


#endif // EM_VERTEX_COLOR_MATERIAL
