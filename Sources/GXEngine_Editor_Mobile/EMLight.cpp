#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXShaderProgram.h>


EMLightEmitter* em_LightEmitters = 0;


EMLightEmitter::EMLightEmitter ()
{
	prev = 0;
	next = em_LightEmitters;
	if ( em_LightEmitters )
		em_LightEmitters->prev = this;
	em_LightEmitters = this;

	type = eEMLightEmitterType::Unknown;

	memset ( baseColor, 255, 3 * sizeof ( GXUByte ) );
	color = GXCreateVec3 ( 1.0f, 1.0f, 1.0f );
	intensity = 1.0f;

	GXSetMat4Identity ( mod_mat );
}

EMLightEmitter::~EMLightEmitter ()
{
	if ( next ) next->prev = prev;
	if ( prev ) 
		prev->next = next;
	else
		em_LightEmitters = next;
}

eEMLightEmitterType EMLightEmitter::GetType ()
{
	return type;
}

GXVoid EMLightEmitter::SetBaseColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	baseColor[ 0 ] = red;
	baseColor[ 1 ] = green;
	baseColor[ 2 ] = blue;

	#define COLOR_FACTOR	0.00392f

	color.r = baseColor[ 0 ] * COLOR_FACTOR * intensity;
	color.g = baseColor[ 1 ] * COLOR_FACTOR * intensity;
	color.b = baseColor[ 2 ] * COLOR_FACTOR * intensity;

	#undef COLOR_FACTOR
}

GXVoid EMLightEmitter::SetIntensity ( GXFloat intensity )
{
	this->intensity = intensity;

	#define COLOR_FACTOR	0.00392f

	color.r = baseColor[ 0 ] * COLOR_FACTOR * intensity;
	color.g = baseColor[ 1 ] * COLOR_FACTOR * intensity;
	color.b = baseColor[ 2 ] * COLOR_FACTOR * intensity;

	#undef COLOR_FACTOR
}

GXVoid EMLightEmitter::GetBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue )
{
	red = baseColor[ 0 ];
	green = baseColor[ 0 ];
	blue = baseColor[ 0 ];
}

const GXVec3& EMLightEmitter::GetColor ()
{
	return color;
}

GXFloat EMLightEmitter::GetIntensity ()
{
	return intensity;
}

//---------------------------------------------------------------

EMBulp::EMBulp ()
{
	type = eEMLightEmitterType::Bulp;
	SetInfluenceDistance ( 1.0f );

	LoadLightVolume ();
}

EMBulp::~EMBulp ()
{
	GXMeshGeometry::RemoveMeshGeometry ( lightVolume );
}

GXVoid EMBulp::SetInfluenceDistance ( GXFloat distance )
{
	mod_mat.m11 = mod_mat.m22 = mod_mat.m33 = distance;
}

GXFloat EMBulp::GetInfluenceDistance ()
{
	return mod_mat.m33;
}

GXVoid EMBulp::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	mod_mat.m41 = x;
	mod_mat.m42 = y;
	mod_mat.m43 = z;
}

GXVoid EMBulp::GetLocation ( GXVec3& location )
{
	mod_mat.GetW ( location );
}

GXVoid EMBulp::DrawLightVolume ()
{
	lightVolume.Render ();
}

GXVoid EMBulp::LoadLightVolume ()
{
	lightVolume = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Bulp light volume.stm" );
}

//---------------------------------------------------------------

EMSpotlight::EMSpotlight ()
{
	type = eEMLightEmitterType::Spot;

	coneAngle = GXDegToRad ( 90.0f );

	GXSetMat4Identity ( rot_mat );
	location = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	compressionXY = 1.0f;

	SetInfluenceDistance ( 1.0f );
	LoadLightVolume ();
}

EMSpotlight::~EMSpotlight ()
{
	GXMeshGeometry::RemoveMeshGeometry ( lightVolume );
}

GXVoid EMSpotlight::SetInfluenceDistance ( GXFloat distance )
{
	this->distance = distance;

	GXMat4 scale_mat;
	GXSetMat4Scale ( scale_mat, distance * compressionXY, distance * compressionXY, distance );

	GXMulMat4Mat4 ( mod_mat, scale_mat, rot_mat );
	mod_mat.SetW ( location );
}

GXVoid EMSpotlight::SetConeAngle ( GXFloat angle_rad )
{
	coneAngle = angle_rad;
	compressionXY = tanf ( coneAngle * 0.5f );
	SetInfluenceDistance ( distance );
}

GXFloat EMSpotlight::GetInfluenceDistance ()
{
	return distance;
}

GXFloat EMSpotlight::GetConeAngle ()
{
	return coneAngle;
}

GXVoid EMSpotlight::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	location.x = x;
	location.y = y;
	location.z = z;

	mod_mat.SetW ( location );
}

GXVoid EMSpotlight::GetLocation ( GXVec3& location )
{
	mod_mat.GetW ( location );
}

GXVoid EMSpotlight::SetRotation ( const GXMat4 &rot )
{
	rot_mat = rot;
	SetInfluenceDistance ( distance );
}

GXVoid EMSpotlight::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, pitch_rad, yaw_rad, roll_rad );
	SetInfluenceDistance ( distance );
}

const GXMat4& EMSpotlight::GetRotation ()
{
	return rot_mat;
}

GXVoid EMSpotlight::DrawLightVolume ()
{
	lightVolume.Render ();
}

GXVoid EMSpotlight::LoadLightVolume ()
{
	lightVolume = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Spot light volume.stm" );
}

//---------------------------------------------------------------

EMDirectedLight::EMDirectedLight ()
{
	type = eEMLightEmitterType::Directed;
	SetRotation ( GXDegToRad ( -90.0f ), 0.0, 0.0 );

	memset ( ambientBase, 0, 3 * sizeof ( GXUByte ) );
	ambientColor = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	ambientIntensity = 0.0f;
}

EMDirectedLight::~EMDirectedLight ()
{
	//NOTHING
}

GXVoid EMDirectedLight::SetAmbientBaseColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	ambientBase[ 0 ] = red;
	ambientBase[ 1 ] = green;
	ambientBase[ 2 ] = blue;

	#define COLOR_FACTOR	0.00392f

	ambientColor.r = ambientBase[ 0 ] * COLOR_FACTOR * ambientIntensity;
	ambientColor.g = ambientBase[ 1 ] * COLOR_FACTOR * ambientIntensity;
	ambientColor.b = ambientBase[ 2 ] * COLOR_FACTOR * ambientIntensity;

	#undef COLOR_FACTOR
}

GXVoid EMDirectedLight::SetAmbientIntensity ( GXFloat intens )
{
	ambientIntensity = intens;

	#define COLOR_FACTOR	0.00392f

	ambientColor.r = ambientBase[ 0 ] * COLOR_FACTOR * ambientIntensity;
	ambientColor.g = ambientBase[ 1 ] * COLOR_FACTOR * ambientIntensity;
	ambientColor.b = ambientBase[ 2 ] * COLOR_FACTOR * ambientIntensity;

	#undef COLOR_FACTOR
}

GXVoid EMDirectedLight::GetAmbientBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue )
{
	red = ambientBase[ 0 ];
	green = ambientBase[ 1 ];
	blue = ambientBase[ 2 ];
}

const GXVec3& EMDirectedLight::GetAmbientColor ()
{
	return ambientColor;
}

GXFloat EMDirectedLight::GetAmbientIntensity ()
{
	return ambientIntensity;
}

GXVoid EMDirectedLight::SetRotation ( const GXMat4 &rot )
{
	mod_mat = rot;
}

GXVoid EMDirectedLight::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( mod_mat, pitch_rad, yaw_rad, roll_rad );
}

const GXMat4& EMDirectedLight::GetRotation ()
{
	return mod_mat;
}
