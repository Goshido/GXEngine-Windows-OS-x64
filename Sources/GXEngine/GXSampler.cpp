// version 1.5

#include <GXEngine/GXSampler.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXUPointerAtomic.h>


#define INVALID_TEXTURE_UNIT    0xFFu

//---------------------------------------------------------------------------------------------------------------------

class GXSamplerEntry final : public GXMemoryInspector
{
    private:
        GXSamplerEntry*             _previous;
        GXSamplerEntry*             _next;
        GXUPointerAtomic            _references;

        GLint                       _wrapMode;
        eGXResampling               _resampling;
        GXFloat                     _anisotropy;

        GLuint                      _samplerObject;

        static GXSamplerEntry*      _top;

    public:
        explicit GXSamplerEntry ( GLint wrapMode, eGXResampling resampling, GXFloat anisotrophy );

        GLuint GetSamplerObject () const;

        GXVoid AddReference ();
        GXVoid Release ();

        // Method returns valid pointer if sampler resource was found.
        // Method returns nullptr if sampler resource was not found.
        static GXSamplerEntry* GXCALL Find ( GLint matchWrapMode, eGXResampling matchResampling, GXFloat matchAnisotrophy );

        static GXUInt GXCALL GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy );

    private:
        ~GXSamplerEntry ();

        GXSamplerEntry () = delete;
        GXSamplerEntry ( const GXSamplerEntry &other ) = delete;
        GXSamplerEntry& operator = ( const GXSamplerEntry &other ) = delete;
};

GXSamplerEntry* GXSamplerEntry::_top = nullptr;

GXSamplerEntry::GXSamplerEntry ( GLint wrapMode, eGXResampling resampling, GXFloat anisotropy )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSamplerEntry" )
    _previous ( nullptr ),
    _next ( _top ),
    _references ( 1u ),
    _wrapMode ( wrapMode ),
    _resampling ( resampling ),
    _anisotropy ( anisotropy )
{
    if ( _top )
        _top->_previous = this;

    _top = this;

    glGenSamplers ( 1, &_samplerObject );

    switch ( resampling )
    {
        case eGXResampling::Linear:
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        break;

        case eGXResampling::Bilinear:
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
        break;

        case eGXResampling::Trilinear:
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        break;

        case eGXResampling::None:
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glSamplerParameteri ( _samplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        break;
    }

    glSamplerParameteri ( _samplerObject, GL_TEXTURE_WRAP_S, wrapMode );
    glSamplerParameteri ( _samplerObject, GL_TEXTURE_WRAP_T, wrapMode );
    glSamplerParameteri ( _samplerObject, GL_TEXTURE_WRAP_R, wrapMode );

    GLfloat maxAnisotropy = 1.0f;
    glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
    GXFloat supportedAnisotropy = ( anisotropy > maxAnisotropy ) ? maxAnisotropy : anisotropy;
    glSamplerParameterf ( _samplerObject, GL_TEXTURE_MAX_ANISOTROPY_EXT, supportedAnisotropy );
}

GLuint GXSamplerEntry::GetSamplerObject () const
{
    return _samplerObject;
}

GXVoid GXSamplerEntry::AddReference ()
{
    ++_references;
}

GXVoid GXSamplerEntry::Release ()
{
    --_references;

    if ( _references > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXSamplerEntry* GXCALL GXSamplerEntry::Find ( GLint matchWrapMode, eGXResampling matchResampling, GXFloat matchAnisotrophy )
{
    for ( GXSamplerEntry* p = _top; p; p = p->_next )
    {
        if ( p->_wrapMode != matchWrapMode || p->_resampling != matchResampling || p->_anisotropy != matchAnisotrophy ) continue;

        return p;
    }

    return nullptr;
}

GXUInt GXCALL GXSamplerEntry::GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy )
{
    GXUInt total = 0u;

    for ( GXSamplerEntry* p = _top; p; p = p->_next )
        ++total;

    if ( total == 0u )
        return 0u;

    lastWrapMode = _top->_wrapMode;
    lastResampling = _top->_resampling;
    lastAnisotropy = _top->_anisotropy;

    return total;
}

GXSamplerEntry::~GXSamplerEntry ()
{
    if ( _top == this )
        _top = _top->_next;

    if ( _previous )
        _previous->_next = _next;

    if ( _next )
        _next->_previous = _previous;

    glDeleteSamplers ( 1, &_samplerObject );
}

//---------------------------------------------------------------------------------------------------------------------

GXSampler::GXSampler ( GLint wrapMode, eGXResampling resampling, GXFloat anisotropy )
{
    _samplerEntry = GXSamplerEntry::Find ( wrapMode, resampling, anisotropy );

    if ( _samplerEntry )
    {
        _samplerEntry->AddReference ();
        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXSamplerEntry" );
    _samplerEntry = new GXSamplerEntry ( wrapMode, resampling, anisotropy );
}

GXSampler::~GXSampler ()
{
    _samplerEntry->Release ();
}

GXVoid GXSampler::Bind ( GXUByte unit )
{
    glBindSampler ( unit, _samplerEntry->GetSamplerObject () );
}

GXVoid GXSampler::Unbind ( GXUByte unit )
{
    glBindSampler ( unit, 0u );
}

GXUInt GXCALL GXSampler::GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy )
{
    return GXSamplerEntry::GetTotalSamplers ( lastWrapMode, lastResampling, lastAnisotropy );
}
