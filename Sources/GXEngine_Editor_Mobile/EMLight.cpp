#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXShaderProgram.h>


#define DEFAULT_COLOR_RED				255
#define DEFAULT_COLOR_GREEN				255
#define DEFAULT_COLOR_BLUE				255
#define DEFAULT_COLOR_ALPHA				255

#define DEFAULT_AMBIENT_COLOR_RED		0
#define DEFAULT_AMBIENT_COLOR_GREEN		0
#define DEFAULT_AMBIENT_COLOR_BLUE		0
#define DEFAULT_AMBIENT_COLOR_ALPHA		0


EMLightEmitter* EMLightEmitter::emitters = nullptr;

EMLightEmitter::EMLightEmitter ()
{
	prev = nullptr;
	next = emitters;

	if ( emitters )
		emitters->prev = this;

	emitters = this;

	type = eEMLightEmitterType::Unknown;

	memset ( baseColor, 255, 3 * sizeof ( GXUByte ) );
	color.From ( DEFAULT_COLOR_RED, DEFAULT_COLOR_GREEN, DEFAULT_COLOR_BLUE, DEFAULT_COLOR_ALPHA );
	intensity = 1.0f;

	mod_mat.Identity ();
}

EMLightEmitter::~EMLightEmitter ()
{
	if ( next ) next->prev = prev;
	if ( prev ) 
		prev->next = next;
	else
		emitters = next;
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

	color.SetRed ( baseColor[ 0 ] * COLOR_FACTOR * intensity );
	color.SetGreen ( baseColor[ 1 ] * COLOR_FACTOR * intensity );
	color.SetBlue ( baseColor[ 2 ] * COLOR_FACTOR * intensity );

	#undef COLOR_FACTOR
}

GXVoid EMLightEmitter::SetIntensity ( GXFloat newIntensity )
{
	intensity = newIntensity;

	#define COLOR_FACTOR	0.00392f

	color.SetRed ( baseColor[ 0 ] * COLOR_FACTOR * intensity );
	color.SetGreen ( baseColor[ 1 ] * COLOR_FACTOR * intensity );
	color.SetBlue ( baseColor[ 2 ] * COLOR_FACTOR * intensity );

	#undef COLOR_FACTOR
}

GXVoid EMLightEmitter::GetBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue )
{
	red = baseColor[ 0 ];
	green = baseColor[ 0 ];
	blue = baseColor[ 0 ];
}

const GXColorRGB& EMLightEmitter::GetColor ()
{
	return color;
}

GXFloat EMLightEmitter::GetIntensity ()
{
	return intensity;
}

EMLightEmitter* EMLightEmitter::GetEmitters ()
{
	return emitters;
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

GXVoid EMBulp::SetInfluenceDistance ( GXFloat newDistance )
{
	mod_mat.m[ 0 ][ 0 ] = mod_mat.m[ 1 ][ 1 ] = mod_mat.m[ 2 ][ 2 ] = newDistance;
}

GXFloat EMBulp::GetInfluenceDistance ()
{
	return mod_mat.m[ 1 ][ 1 ];
}

GXVoid EMBulp::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	mod_mat.m[ 3 ][ 0 ] = x;
	mod_mat.m[ 3 ][ 1 ] = y;
	mod_mat.m[ 3 ][ 2 ] = z;
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
	lightVolume = GXMeshGeometry::LoadFromStm ( L"Meshes/Editor Mobile/Bulp light volume.stm" );
}

//---------------------------------------------------------------

EMSpotlight::EMSpotlight ()
{
	type = eEMLightEmitterType::Spot;

	coneAngle = GXDegToRad ( 90.0f );

	rot_mat.Identity ();
	location.Init ( 0.0f, 0.0f, 0.0f );
	compressionXY = 1.0f;

	SetInfluenceDistance ( 1.0f );
	LoadLightVolume ();
}

EMSpotlight::~EMSpotlight ()
{
	GXMeshGeometry::RemoveMeshGeometry ( lightVolume );
}

GXVoid EMSpotlight::SetInfluenceDistance ( GXFloat newDistance )
{
	distance = newDistance;

	GXMat4 scale_mat;
	scale_mat.Scale ( distance * compressionXY, distance * compressionXY, distance );

	mod_mat.Multiply ( scale_mat, rot_mat );
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
	location.Init ( x, y, z );
	mod_mat.SetW ( location );
}

GXVoid EMSpotlight::GetLocation ( GXVec3& out )
{
	mod_mat.GetW ( out );
}

GXVoid EMSpotlight::SetRotation ( const GXMat4 &rot )
{
	rot_mat = rot;
	SetInfluenceDistance ( distance );
}

GXVoid EMSpotlight::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
	rot_mat.RotationXYZ ( pitchRadians, yawRadians, rollRadians );
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
	lightVolume = GXMeshGeometry::LoadFromStm ( L"Meshes/Editor Mobile/Spot light volume.stm" );
}

//---------------------------------------------------------------

EMDirectedLight::EMDirectedLight ()
{
	type = eEMLightEmitterType::Directed;
	SetRotation ( GXDegToRad ( -90.0f ), 0.0, 0.0 );

	memset ( ambientBase, 0, 3 * sizeof ( GXUByte ) );
	ambientColor.From ( DEFAULT_AMBIENT_COLOR_RED, DEFAULT_AMBIENT_COLOR_GREEN, DEFAULT_AMBIENT_COLOR_BLUE, DEFAULT_AMBIENT_COLOR_ALPHA );
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

	ambientColor.SetRed ( ambientBase[ 0 ] * COLOR_FACTOR * ambientIntensity );
	ambientColor.SetGreen ( ambientBase[ 1 ] * COLOR_FACTOR * ambientIntensity );
	ambientColor.SetBlue ( ambientBase[ 2 ] * COLOR_FACTOR * ambientIntensity );

	#undef COLOR_FACTOR
}

GXVoid EMDirectedLight::SetAmbientIntensity ( GXFloat intens )
{
	ambientIntensity = intens;

	#define COLOR_FACTOR	0.00392f

	ambientColor.SetRed ( ambientBase[ 0 ] * COLOR_FACTOR * ambientIntensity );
	ambientColor.SetGreen ( ambientBase[ 1 ] * COLOR_FACTOR * ambientIntensity );
	ambientColor.SetBlue ( ambientBase[ 2 ] * COLOR_FACTOR * ambientIntensity );

	#undef COLOR_FACTOR
}

GXVoid EMDirectedLight::GetAmbientBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue )
{
	red = ambientBase[ 0 ];
	green = ambientBase[ 1 ];
	blue = ambientBase[ 2 ];
}

const GXColorRGB& EMDirectedLight::GetAmbientColor ()
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

GXVoid EMDirectedLight::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
	mod_mat.RotationXYZ ( pitchRadians, yawRadians, rollRadians );
}

const GXMat4& EMDirectedLight::GetRotation ()
{
	return mod_mat;
}
