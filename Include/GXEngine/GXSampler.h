// version 1.3

#ifndef GX_SAMPLER
#define GX_SAMPLER


#include "GXOpenGL.h"


enum class eGXResampling : GXUByte
{
    None,
    Linear,
    Bilinear,
    Trilinear
};

// Class handles lazy loading reference counting sampler resource creation.

class GXSamplerEntry;
class GXSampler final
{
    private:
        GXSamplerEntry*     samplerEntry;

    public:
        explicit GXSampler ( GLint wrapMode, eGXResampling resampling, GXFloat anisotropy );
        ~GXSampler ();

        GXVoid Bind ( GXUByte unit );
        GXVoid Unbind ( GXUByte unit );

        static GXUInt GXCALL GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy );

    private:
        GXSampler () = delete;
        GXSampler ( const GXSampler &other ) = delete;
        GXSampler& operator = ( const GXSampler &other ) = delete;
};


#endif // GX_SAMPLER
