// version 1.0

#include <GXPhysics/GXFirework.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXMemory.h>


#define MAX_FIREWORKS	1024
#define RULE_COUNT		3


GXVoid GXFirework::SetType ( GXUInt type )
{
	this->type = type;
}

GXUInt GXFirework::GetType () const
{
	return type;
}

GXVoid GXFirework::SetAge ( GXFloat age )
{
	this->age = age;
}

GXFloat GXFirework::GetAge () const
{
	return age;
}

GXBool GXFirework::Update ( GXFloat deltaTime )
{
	Integrate ( deltaTime );

	age -= deltaTime;
	return age < 0.0f;
}

//-------------------------------------------------------------------------

GXFireworkRule::GXFireworkRule ()
{
	payloadCount = 0;
	payloads = nullptr;
}

GXFireworkRule::~GXFireworkRule ()
{
	GXSafeFree ( payloads );
	payloadCount = 0;
}

GXVoid GXFireworkRule::Init ( GXUInt payloadCount )
{
	this->payloadCount = payloadCount;
	payloads = (Payload*)malloc ( payloadCount * sizeof ( Payload ) );
}

GXVoid GXFireworkRule::SetParameters ( GXUInt type, GXFloat minAge, GXFloat maxAge, const GXVec3 &minVelocity, const GXVec3 &maxVelocity, GXFloat damping )
{
	this->type = type;
	this->minAge = minAge;
	this->maxAge = maxAge;

	this->minVelocity = minVelocity;
	this->maxVelocity = maxVelocity;

	this->damping = damping;
}

GXVoid GXFireworkRule::Create ( GXFirework& firework, const GXFirework* parent ) const
{
	firework.SetType ( type );
	firework.SetAge ( GXRandomBetween ( minAge, maxAge ) );


	GXVec3 velocity ( 0.0f, 0.0f, 0.0f );

	if ( parent )
	{
		firework.SetLocation ( parent->GetLocation () );
		velocity = parent->GetVelocity ();
	}
	else
	{
		GXVec3 start (0.0f, 0.0f, 0.0f);
		start.x = 5.0f * GXRandomNormalize ();
		firework.SetLocation(start);
	}

	GXVec3 velocityDelta;
	GXRandomBetween ( velocityDelta, minVelocity, maxVelocity );
	GXSumVec3Vec3 ( velocity, velocity, velocityDelta );
	firework.SetVelocity ( velocity );

	firework.SetMass ( 1.0 );
	firework.SetDumping ( damping );
	firework.SetAcceleration ( GXPhysicsEngine::GetInstance ()->GetGravity () );
	firework.ClearForceAccumulator ();
}

//-------------------------------------------------------------------------

GXFireworkRule gx_phs_FireworksRules[ RULE_COUNT ];

GXFirework gx_phs_Fireworks[ MAX_FIREWORKS ];
GXUInt gx_phs_NextFirework = 0;
PFNGXONFRAMEWORKSTATEPROC GXPHSDisplayCallback = nullptr;

void GXCreateFireworkDemo ( GXUInt type, GXUInt number, const GXFirework* parent );

GXVoid GXCALL GXInitFireworkDemo ( PFNGXONFRAMEWORKSTATEPROC displayCallback )
{
	gx_phs_FireworksRules[ 0 ].Init ( 2 );
	gx_phs_FireworksRules[ 0 ].payloads[ 0 ].Set ( 2, 5 );
    gx_phs_FireworksRules[ 0 ].payloads[ 1 ].Set ( 1, 5 );
	gx_phs_FireworksRules[ 0 ].SetParameters ( 1, 3.0f, 5.0f, GXVec3 ( -5.0f, -5.0f, -5.0f ), GXVec3 ( 5.0f, 5.0f, 5.0f ), 0.1f );
	
	gx_phs_FireworksRules[ 1 ].Init ( 1 );
    gx_phs_FireworksRules[ 1 ].payloads[ 0 ].Set ( 3, 10 );
	gx_phs_FireworksRules[ 1 ].SetParameters ( 2, 4.0f, 5.0f, GXVec3 ( -1.0f, -5.0f, -3.0f ), GXVec3 ( 0.0f, 1.0f, 7.0f ), 0.1f );
	
	gx_phs_FireworksRules[ 2 ].Init ( 0 );
	gx_phs_FireworksRules[ 2 ].SetParameters ( 3, 1.0f, 5.0f, GXVec3 ( 1.0f, -8.0f, -5.0f ), GXVec3 ( 5.0f, 0.0f, 10.0f ), 0.25f );

	GXCreateFireworkDemo ( 1, 1, nullptr );

	GXPHSDisplayCallback = displayCallback;
}

GXBool GXCALL GXIsFireworkDemoStarted ()
{
	return GXPHSDisplayCallback != nullptr;
}

GXVoid GXCALL GXCreateFireworkDemo ( GXUInt type, const GXFirework* parent )
{
	gx_phs_FireworksRules[ type - 1 ].Create ( gx_phs_Fireworks[ gx_phs_NextFirework ], parent );
	gx_phs_NextFirework = ( gx_phs_NextFirework + 1 ) % MAX_FIREWORKS;
}

void GXCreateFireworkDemo ( GXUInt type, GXUInt number, const GXFirework* parent )
{
	for ( GXUInt i = 0; i < number; i++ )
		GXCreateFireworkDemo ( type, parent );
}

GXVoid GXCALL GXUpdateFirework ( GXFloat deltaTime )
{
	for ( GXUInt i = 0; i < MAX_FIREWORKS; i++ )
	{
		GXFirework& firework = gx_phs_Fireworks[ i ];

		if ( firework.GetType () > 0 )
		{
			if ( GXPHSDisplayCallback )
				GXPHSDisplayCallback ( firework.GetLocation (), firework.GetAge () );

			if ( firework.Update ( deltaTime ) )
			{
				GXFireworkRule* rule = gx_phs_FireworksRules + ( firework.GetType () - 1 );
				firework.SetType ( 0 );

				for ( GXUInt i = 0; i < rule->payloadCount; i++ )
				{
					GXFireworkRule::Payload* payload = rule->payloads + i;
					GXCreateFireworkDemo ( payload->type, payload->count, &firework );
				}
			}
		}

	}
}