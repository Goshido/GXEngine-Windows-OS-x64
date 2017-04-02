//version 1.7

#ifndef EM_LIGHT
#define EM_LIGHT


#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXMath.h>


class EMLightEmitter;
extern EMLightEmitter* em_LightEmitters;

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
		EMLightEmitter*		next;
		EMLightEmitter*		prev;

	protected:
		eEMLightEmitterType	type;

		GXUByte				baseColor[ 3 ];
		GXFloat				intensity;		//[ 0.0f, +inf )
		GXVec3				color;

		GXMat4				mod_mat;

	public:
		EMLightEmitter ();
		virtual ~EMLightEmitter ();

		eEMLightEmitterType GetType ();
		GXVoid SetBaseColor ( GXUByte r, GXUByte g, GXUByte b );
		GXVoid SetIntensity ( GXFloat intensity );

		GXVoid GetBaseColor ( GXUByte &r, GXUByte &g, GXUByte &b );
		const GXVec3& GetColor ();
		GXFloat GetIntensity ();
};

class EMBulp : public EMLightEmitter
{
	private:
		GXMeshGeometry	lightVolume;

	public:
		EMBulp ();
		virtual ~EMBulp ();

		GXVoid SetInfluenceDistance ( GXFloat distance );
		GXFloat GetInfluenceDistance ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		const GXVec3& GetLocation ();

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
		virtual ~EMSpotlight ();

		GXVoid SetInfluenceDistance ( GXFloat distance );
		GXVoid SetConeAngle ( GXFloat angle_rad );

		GXFloat GetInfluenceDistance ();
		GXFloat GetConeAngle ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		const GXVec3& GetLocation ();

		GXVoid SetRotation ( const GXMat4 &rot );
		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );

		const GXMat4& GetRotation ();

		GXVoid DrawLightVolume ();

	protected:
		GXVoid LoadLightVolume ();
};

class EMDirectedLight : public EMLightEmitter
{
	private:
		GXUByte	ambientBase[ 3 ];
		GXFloat	ambientIntensity;
		GXVec3	ambientColor;

	public:
		EMDirectedLight ();
		virtual ~EMDirectedLight ();

		GXVoid SetAmbientBaseColor ( GXUByte r, GXUByte g, GXUByte b );
		GXVoid SetAmbientIntensity ( GXFloat intens );

		GXVoid GetAmbientBaseColor ( GXUByte &r, GXUByte &g, GXUByte &b );
		const GXVec3& GetAmbientColor ();
		GXFloat GetAmbientIntensity ();

		GXVoid SetRotation ( const GXMat4 &rot );
		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );

		const GXMat4& GetRotation ();
};


#endif //EM_LIGHT
