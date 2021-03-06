#ifndef EM_TONE_MAPPER_LIMINANCE_TRIPLET_REDUCER_MATERIAL
#define EM_TONE_MAPPER_LIMINANCE_TRIPLET_REDUCER_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMToneMapperLuminanceTripletReducerMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _luminanceTripletTexture;
        GXSampler       _sampler;
        GXFloat         _levelOfDetail;

        GLint           _levelOfDetailLocation;

    public:
        EMToneMapperLuminanceTripletReducerMaterial ();
        ~EMToneMapperLuminanceTripletReducerMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetLuminanceTripletTexture ( GXTexture2D &texture );
        GXVoid SetLevelOfDetailToReduce ( GXUByte newLevelOfDetail );

    private:
        EMToneMapperLuminanceTripletReducerMaterial ( const EMToneMapperLuminanceTripletReducerMaterial &other ) = delete;
        EMToneMapperLuminanceTripletReducerMaterial& operator = ( const EMToneMapperLuminanceTripletReducerMaterial &other ) = delete;
};


#endif // EM_TONE_MAPPER_LIMINANCE_TRIPLET_REDUCER_MATERIAL
