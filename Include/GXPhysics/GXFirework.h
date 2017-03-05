// version 1.0

#ifndef GX_FIREWORK
#define GX_FIREWORK


#include <GXPhysics/GXParticle.h>


typedef GXVoid ( GXCALL* PFNGXONFRAMEWORKSTATEPROC ) ( const GXVec3& location, GXFloat age );


class GXFirework : public GXParticle
{
	protected:
		GXUInt		type;
		GXFloat		age;

	public:
		GXVoid SetType ( GXUInt type );
		GXUInt GetType () const;

		GXVoid SetAge ( GXFloat age );
		GXFloat GetAge () const;

		GXBool Update ( GXFloat deltaTime );
};

struct GXFireworkRule
{
	GXUInt		type;
	GXFloat		minAge;
	GXFloat		maxAge;

	GXVec3		minVelocity;
	GXVec3		maxVelocity;

	GXFloat		damping;

	struct Payload
	{
		GXUInt		type;
		GXUInt		count;

		GXVoid Set ( GXUInt type, GXUInt count )
		{
			Payload::type = type;
			Payload::count = count;
		}
	};

	GXUInt		payloadCount;

	Payload*	payloads;

	GXFireworkRule ();
	~GXFireworkRule ();

	GXVoid Init ( GXUInt payloadCount );

	GXVoid SetParameters ( GXUInt type, GXFloat nimAge, GXFloat maxAge, const GXVec3 &minVelocity, const GXVec3 &maxVelocity, GXFloat damping );
	GXVoid Create ( GXFirework& firework, const GXFirework* parent = nullptr ) const;
};

GXVoid GXCALL GXInitFireworkDemo( PFNGXONFRAMEWORKSTATEPROC displayCallback );
GXBool GXCALL GXIsFireworkDemoStarted ();
GXVoid GXCALL GXUpdateFirework ( GXFloat deltaTime );


#endif //GX_FIREWORK
