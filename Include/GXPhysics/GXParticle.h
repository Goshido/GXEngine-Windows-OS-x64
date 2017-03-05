// vesrsion 1.0

#ifndef GX_PARTICLE_1
#define GX_PARTICLE_1


#include <GXCommon/GXMath.h>


class GXParticle
{
	protected:
		GXVec3		location;
		GXVec3		velocity;
		GXVec3		acceleration;

		GXFloat		damping;

		GXFloat		mass;
		GXFloat		invMass;

		GXVec3		totalForce;

	public:
		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &location );
		const GXVec3& GetLocation () const;

		GXVoid SetVelocity ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetVelocity ( const GXVec3 &velocity );
		const GXVec3& GetVelocity () const;

		GXVoid SetAcceleration ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetAcceleration ( const GXVec3 &acceleration );
		const GXVec3& GetAcceleration () const;

		GXVoid SetMass ( GXFloat mass );
		GXFloat GetMass () const;
		GXFloat GetInverseMass () const;
		GXBool HasFiniteMass ();

		GXVoid SetDumping ( GXFloat damping );

		GXVoid AddForce ( const GXVec3 &force );
		GXVoid ClearForceAccumulator ();

		GXVoid Integrate ( GXFloat deltaTime );
};


#endif //GX_PARTICLE_1
