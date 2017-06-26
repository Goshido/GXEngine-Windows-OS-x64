#ifndef EM_SSAO_SHARP_MATERIAL
#define EM_SSAO_SHARP_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


#define EM_MAX_SSAO_SAMPLES	64


class EMSSAOSharpMaterial : public GXMaterial
{
	private:
		GXTexture*		depthTexture;
		GXTexture*		normalTexture;
		GXTexture		noiseTexture;

		GXVec3			kernel[ EM_MAX_SSAO_SAMPLES ];
		GLint			kernelLocation;

		GXFloat			checkRadius;
		GLint			checkRadiusLocation;

		GXInt			samples;
		GLint			samplesLocation;

		GXFloat			inverseSamples;
		GLint			inverseSamplesLocation;

		GXVec2			noiseScale;
		GLint			noiseScaleLocation;

		GXFloat			maxDistance;
		GLint			maxDistanceLocation;

		GLint			projectionMatrixLocation;
		GLint			inverseProjectionMatrixLocation;

	public:
		EMSSAOSharpMaterial ();
		~EMSSAOSharpMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetDepthTexture ( GXTexture &texture );
		GXVoid SetNormalTexture ( GXTexture &texture );

		GXVoid SetCheckRadius ( GXFloat meters );
		GXFloat GetCheckRadius () const;

		GXVoid SetSampleNumber ( GXUByte samples );
		GXUByte GetSampleNumber () const;

		GXVoid SetNoiseTextureResolution ( GXUShort resolution );
		GXUShort GetNoiseTextureResolution () const;

		GXVoid SetMaximumDistance ( GXFloat meters );
		GXFloat GetMaximumDistance () const;
};


#endif //EM_SSAO_SHARP_MATERIAL
