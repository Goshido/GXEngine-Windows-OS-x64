#ifndef EM_SSAO_SHARP_MATERIAL
#define EM_SSAO_SHARP_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


#define EM_MAX_SSAO_SAMPLES		64u


class EMSSAOSharpMaterial : public GXMaterial
{
	private:
		GXTexture2D*	depthTexture;
		GXTexture2D*	normalTexture;

		GXSampler		sampler;

		GXFloat			maxDistance;
		GXFloat			checkRadius;

		GXTexture2D		noiseTexture;

		GXInt			samples;
		GXFloat			inverseSamples;

		GXVec2			noiseScale;
		GXVec3			kernel[ EM_MAX_SSAO_SAMPLES ];

		GLint			checkRadiusLocation;
		GLint			kernelLocation;
		GLint			samplesLocation;
		GLint			inverseSamplesLocation;
		GLint			noiseScaleLocation;
		GLint			maxDistanceLocation;
		GLint			projectionMatrixLocation;
		GLint			inverseProjectionMatrixLocation;

	public:
		EMSSAOSharpMaterial ();
		~EMSSAOSharpMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetDepthTexture ( GXTexture2D &texture );
		GXVoid SetNormalTexture ( GXTexture2D &texture );

		GXVoid SetCheckRadius ( GXFloat meters );
		GXFloat GetCheckRadius () const;

		GXVoid SetSampleNumber ( GXUByte samplesPerPixel );
		GXUByte GetSampleNumber () const;

		GXVoid SetNoiseTextureResolution ( GXUShort resolution );
		GXUShort GetNoiseTextureResolution () const;

		GXVoid SetMaximumDistance ( GXFloat meters );
		GXFloat GetMaximumDistance () const;

	private:
		EMSSAOSharpMaterial ( const EMSSAOSharpMaterial &other ) = delete;
		EMSSAOSharpMaterial& operator = ( const EMSSAOSharpMaterial &other ) = delete;
};


#endif // EM_SSAO_SHARP_MATERIAL
