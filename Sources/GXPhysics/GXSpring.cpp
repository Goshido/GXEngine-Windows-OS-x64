// version 1.2

#include <GXPhysics/GXSpring.h>


GXSpring::GXSpring ( const GXVec3 &connectionPointLocal, GXRigidBody &otherBody, const GXVec3 &otherConnectionPointLocal, GXFloat hardness, GXFloat restLength )
{
	this->connectionPointLocal = connectionPointLocal;
	this->otherBody = &otherBody;
	this->otherConnectionPointLocal = otherConnectionPointLocal;
	this->hardness = hardness;
	this->restLength = restLength;
}

GXSpring::~GXSpring ()
{
	// NOTHING
}

GXVoid GXSpring::UpdateForce ( GXRigidBody &body, GXFloat /*deltaTime*/ )
{
	GXVec3 a;
	body.TranslatePointToWorld ( a, connectionPointLocal );

	GXVec3 b;
	otherBody->TranslatePointToWorld ( b, otherConnectionPointLocal );

	GXVec3 delta;
	delta.Substract ( a, b );

	GXFloat stress = fabsf ( delta.Length () - restLength ) * hardness;
	delta.Normalize ();

	GXVec3 force;
	force.Multiply ( delta, -stress );

	body.AddForceAtPointWorld ( force, a );
}
