#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXShaderProgram.h>


#define DEFAULT_COLOR_RED							255u
#define DEFAULT_COLOR_GREEN							255u
#define DEFAULT_COLOR_BLUE							255u
#define DEFAULT_COLOR_ALPHA							255u

#define DEFAULT_AMBIENT_COLOR_RED					0u
#define DEFAULT_AMBIENT_COLOR_GREEN					0u
#define DEFAULT_AMBIENT_COLOR_BLUE					0u
#define DEFAULT_AMBIENT_COLOR_ALPHA					0u

#define DEFAULT_INTENSITY							1.0f

#define DEFAULT_BULP_INFLUENSE_DISTANCE				1.0f

#define DEFAULT_SPOT_LIGHT_CONE_ANGLE				90.0f
#define DEFAULT_SPOT_LIGHT_COMPRESSION_XY			1.0f
#define DEFAULT_SPOT_LIGHT_INFLUENSE_DISTANCE		10.0f

#define DEFAULT_SPOT_LIGHT_LOCATION_X				0.0f
#define DEFAULT_SPOT_LIGHT_LOCATION_Y				0.0f
#define DEFAULT_SPOT_LIGHT_LOCATION_Z				0.0f

#define DEFAULT_SPOT_LIGHT_ROTATION_PITCH			0.0f
#define DEFAULT_SPOT_LIGHT_ROTATION_YAW				0.0f
#define DEFAULT_SPOT_LIGHT_ROTATION_ROLL			0.0f

#define DEFAULT_DIRECTED_LIGHT_AMBIENT_RED			0u
#define DEFAULT_DIRECTED_LIGHT_AMBIENT_GREEN		0u
#define DEFAULT_DIRECTED_LIGHT_AMBIENT_BLUE			0u
#define DEFAULT_DIRECTED_LIGHT_AMBIENT_ALPHA		255u

#define DEFAULT_DIRECTED_LIGHT_AMBIENT_INTENSITY	0.0f

#define DEFAULT_DIRECTED_LIGHT_ROTATION_PITCH		GX_MATH_HALF_PI
#define DEFAULT_DIRECTED_LIGHT_ROTATION_YAW			0.0f
#define DEFAULT_DIRECTED_LIGHT_ROTATION_ROLL		0.0f

#define COLOR_FACTOR								0.00392f

#define BULP_LIGHT_VOLUME_MESH						L"Meshes/Editor Mobile/Bulp light volume.stm"
#define SPOT_LIGHT_VOLUME_MESH						L"Meshes/Editor Mobile/Spot light volume.stm"

//---------------------------------------------------------------

EMLightEmitter* EMLightEmitter::emitters = nullptr;

EMLightEmitter::EMLightEmitter ():
	baseColor ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) ),
	prev ( nullptr )
{
	next = emitters;

	if ( emitters )
		emitters->prev = this;

	emitters = this;

	SetIntensity ( DEFAULT_INTENSITY );
	modelMatrix.Identity ();
}

EMLightEmitter::~EMLightEmitter ()
{
	if ( next ) next->prev = prev;

	if ( prev ) 
		prev->next = next;
	else
		emitters = next;
}

eEMLightEmitterType EMLightEmitter::GetType () const
{
	return eEMLightEmitterType::Unknown;
}

GXVoid EMLightEmitter::GetBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const
{
	red = static_cast<GXUByte> ( baseColor._data[ 0 ] * 255.0f );
	green = static_cast<GXUByte> ( baseColor._data[ 1 ] * 255.0f );
	blue = static_cast<GXUByte> ( baseColor._data[ 2 ] * 255.0f );
}

GXVoid EMLightEmitter::SetBaseColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	baseColor.From ( red, green, blue, static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) );

	color._data[ 0 ] = ( baseColor._data[ 0 ] * intensity );
	color._data[ 1 ] = ( baseColor._data[ 1 ] * intensity );
	color._data[ 2 ] = ( baseColor._data[ 2 ] * intensity );
}

GXFloat EMLightEmitter::GetIntensity () const
{
	return intensity;
}

GXVoid EMLightEmitter::SetIntensity ( GXFloat newIntensity )
{
	intensity = newIntensity;

	color._data[ 0 ] = ( baseColor._data[ 0 ] * intensity );
	color._data[ 1 ] = ( baseColor._data[ 1 ] * intensity );
	color._data[ 2 ] = ( baseColor._data[ 2 ] * intensity );
}

const GXColorRGB& EMLightEmitter::GetColor () const
{
	return color;
}

EMLightEmitter* EMLightEmitter::GetEmitters ()
{
	return emitters;
}

//---------------------------------------------------------------

EMBulp::EMBulp ()
{
	SetInfluenceDistance ( DEFAULT_BULP_INFLUENSE_DISTANCE );
	lightVolume.LoadMesh ( BULP_LIGHT_VOLUME_MESH );
}

EMBulp::~EMBulp ()
{
	// NOTHING
}

eEMLightEmitterType EMBulp::GetType () const
{
	return eEMLightEmitterType::Bulp;
}

GXVoid EMBulp::SetInfluenceDistance ( GXFloat newDistance )
{
	modelMatrix._m[ 0 ][ 0 ] = modelMatrix._m[ 1 ][ 1 ] = modelMatrix._m[ 2 ][ 2 ] = newDistance;
}

GXFloat EMBulp::GetInfluenceDistance ()
{
	return modelMatrix._m[ 1 ][ 1 ];
}

GXVoid EMBulp::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	modelMatrix._m[ 3 ][ 0 ] = x;
	modelMatrix._m[ 3 ][ 1 ] = y;
	modelMatrix._m[ 3 ][ 2 ] = z;
}

GXVoid EMBulp::SetLocation ( const GXVec3 &location )
{
	modelMatrix.SetW ( location );
}

GXVoid EMBulp::GetLocation ( GXVec3& location )
{
	modelMatrix.GetW ( location );
}

GXVoid EMBulp::DrawLightVolume ()
{
	lightVolume.Render ();
}

//---------------------------------------------------------------

EMSpotlight::EMSpotlight ():
	coneAngle ( GXDegToRad ( DEFAULT_SPOT_LIGHT_CONE_ANGLE ) ),
	compressionXY ( DEFAULT_SPOT_LIGHT_COMPRESSION_XY )
{
	modelMatrix._m[ 3 ][ 0 ] = DEFAULT_SPOT_LIGHT_LOCATION_X;
	modelMatrix._m[ 3 ][ 1 ] = DEFAULT_SPOT_LIGHT_LOCATION_Y;
	modelMatrix._m[ 3 ][ 2 ] = DEFAULT_SPOT_LIGHT_LOCATION_Z;

	rotationMatrix.RotationXYZ ( DEFAULT_SPOT_LIGHT_ROTATION_PITCH, DEFAULT_SPOT_LIGHT_ROTATION_YAW, DEFAULT_SPOT_LIGHT_ROTATION_ROLL );

	SetInfluenceDistance ( DEFAULT_SPOT_LIGHT_INFLUENSE_DISTANCE );
	lightVolume.LoadMesh ( SPOT_LIGHT_VOLUME_MESH );
}

EMSpotlight::~EMSpotlight ()
{
	// NOTHING
}

eEMLightEmitterType EMSpotlight::GetType () const
{
	return eEMLightEmitterType::Spot;
}

GXFloat EMSpotlight::GetConeAngle () const
{
	return coneAngle;
}

GXVoid EMSpotlight::SetConeAngle ( GXFloat radians )
{
	coneAngle = radians;
	compressionXY = tanf ( coneAngle * 0.5f );
	SetInfluenceDistance ( distance );
}

GXVoid EMSpotlight::SetInfluenceDistance ( GXFloat newDistance )
{
	distance = newDistance;

	GXMat4 scaleMatrix;
	scaleMatrix.Scale ( distance * compressionXY, distance * compressionXY, distance );

	GXVec3 location;
	modelMatrix.GetW ( location );

	modelMatrix.Multiply ( scaleMatrix, rotationMatrix );
	modelMatrix.SetW ( location );
}

GXFloat EMSpotlight::GetInfluenceDistance ()
{
	return distance;
}

GXVoid EMSpotlight::GetLocation ( GXVec3 &out ) const
{
	modelMatrix.GetW ( out );
}

GXVoid EMSpotlight::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	modelMatrix._m[ 3 ][ 0 ] = x;
	modelMatrix._m[ 3 ][ 1 ] = y;
	modelMatrix._m[ 3 ][ 2 ] = z;
}

GXVoid EMSpotlight::SetLocation ( const GXVec3 &location )
{
	modelMatrix.SetW ( location );
}

const GXMat4& EMSpotlight::GetRotation () const
{
	return rotationMatrix;
}

GXVoid EMSpotlight::SetRotation ( const GXMat4 &rotation )
{
	rotationMatrix = rotation;
	SetInfluenceDistance ( distance );
}

GXVoid EMSpotlight::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
	rotationMatrix.RotationXYZ ( pitchRadians, yawRadians, rollRadians );
	SetInfluenceDistance ( distance );
}

GXVoid EMSpotlight::DrawLightVolume ()
{
	lightVolume.Render ();
}

//---------------------------------------------------------------

EMDirectedLight::EMDirectedLight ():
	ambientBase ( static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_ALPHA ) )
{ 
	SetRotation ( DEFAULT_DIRECTED_LIGHT_ROTATION_PITCH, DEFAULT_DIRECTED_LIGHT_ROTATION_YAW, DEFAULT_DIRECTED_LIGHT_ROTATION_ROLL );
	SetAmbientIntensity ( DEFAULT_DIRECTED_LIGHT_AMBIENT_INTENSITY );
}

EMDirectedLight::~EMDirectedLight ()
{
	// NOTHING
}

eEMLightEmitterType EMDirectedLight::GetType () const
{
	return eEMLightEmitterType::Directed;
}

GXVoid EMDirectedLight::SetAmbientBaseColor ( GXUByte red, GXUByte green, GXUByte blue )
{
	ambientBase.From ( red, green, blue, static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_ALPHA ) );

	ambientColor._data[ 0 ] = ambientBase._data[ 0 ] * ambientIntensity;
	ambientColor._data[ 1 ] = ambientBase._data[ 1 ] * ambientIntensity;
	ambientColor._data[ 2 ] = ambientBase._data[ 2 ] * ambientIntensity;
}

GXVoid EMDirectedLight::SetAmbientIntensity ( GXFloat newIntensity )
{
	ambientIntensity = newIntensity;

	ambientColor._data[ 0 ] = ambientBase._data[ 0 ] * ambientIntensity;
	ambientColor._data[ 1 ] = ambientBase._data[ 1 ] * ambientIntensity;
	ambientColor._data[ 2 ] = ambientBase._data[ 2 ] * ambientIntensity;
}

GXVoid EMDirectedLight::GetAmbientBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue )
{
	red = static_cast<GXUByte> ( ambientBase._data[ 0 ] * 255.0f );
	green = static_cast<GXUByte> ( ambientBase._data[ 1 ] * 255.0f );
	blue = static_cast<GXUByte> ( ambientBase._data[ 2 ] * 255.0f );
}

const GXColorRGB& EMDirectedLight::GetAmbientColor ()
{
	return ambientColor;
}

GXFloat EMDirectedLight::GetAmbientIntensity ()
{
	return ambientIntensity;
}

GXVoid EMDirectedLight::SetRotation ( const GXMat4 &rotation )
{
	modelMatrix = rotation;
}

GXVoid EMDirectedLight::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
	modelMatrix.RotationXYZ ( pitchRadians, yawRadians, rollRadians );
}

const GXMat4& EMDirectedLight::GetRotation ()
{
	return modelMatrix;
}
