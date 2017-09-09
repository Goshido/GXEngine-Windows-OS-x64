//version 1.7

#ifndef EM_LIGHT
#define EM_LIGHT


#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXMath.h>


enum class eEMLightEmitterType
{
	Bulp,
	Directed,
	Spot,
	Unknown
};

class EMLightEmitter
{
	friend class EMRenderer;

	private:
		EMLightEmitter*			next;
		EMLightEmitter*			prev;

		static EMLightEmitter*	emitters;

	protected:
		eEMLightEmitterType		type;

		GXUByte					baseColor[ 3 ];
		GXFloat					intensity;		//[ 0.0f, +inf )
		GXColorRGB				color;

		GXMat4					mod_mat;

	public:
		EMLightEmitter ();
		virtual ~EMLightEmitter ();

		eEMLightEmitterType GetType ();
		GXVoid SetBaseColor ( GXUByte red, GXUByte green, GXUByte blue );
		GXVoid SetIntensity ( GXFloat newIntensity );

		GXVoid GetBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue );
		const GXColorRGB& GetColor ();
		GXFloat GetIntensity ();

	private:
		static EMLightEmitter* GetEmitters ();
};

class EMBulp : public EMLightEmitter
{
	private:
		GXMeshGeometry	lightVolume;

	public:
		EMBulp ();
		~EMBulp () override;

		GXVoid SetInfluenceDistance ( GXFloat newDistance );
		GXFloat GetInfluenceDistance ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid GetLocation ( GXVec3& location);

		GXVoid DrawLightVolume ();

	protected:
		GXVoid LoadLightVolume ();
};

class EMSpotlight : public EMLightEmitter
{
	private:
		GXFloat			distance;
		GXFloat			coneAngle;

		GXFloat			compressionXY;

		GXMat4			rot_mat;
		GXVec3			location;

		GXMeshGeometry	lightVolume;

	public:
		EMSpotlight ();
		~EMSpotlight () override;

		GXVoid SetInfluenceDistance ( GXFloat newDistance );
		GXVoid SetConeAngle ( GXFloat angle_rad );

		GXFloat GetInfluenceDistance ();
		GXFloat GetConeAngle ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid GetLocation ( GXVec3& out );

		GXVoid SetRotation ( const GXMat4 &rot );
		GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

		const GXMat4& GetRotation ();

		GXVoid DrawLightVolume ();

	protected:
		GXVoid LoadLightVolume ();
};

class EMDirectedLight : public EMLightEmitter
{
	private:
		GXUByte		ambientBase[ 3 ];
		GXFloat		ambientIntensity;
		GXColorRGB	ambientColor;

	public:
		EMDirectedLight ();
		~EMDirectedLight () override;

		GXVoid SetAmbientBaseColor ( GXUByte red, GXUByte green, GXUByte blue );
		GXVoid SetAmbientIntensity ( GXFloat intens );

		GXVoid GetAmbientBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue );
		const GXColorRGB& GetAmbientColor ();
		GXFloat GetAmbientIntensity ();

		GXVoid SetRotation ( const GXMat4 &rot );
		GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

		const GXMat4& GetRotation ();
};


#endif //EM_LIGHT
