//version 1.0

#include <GXPhysics/GXSpring.h>


GXSpring::GXSpring ( const GXVec3 &connectionPointLocal, GXRigidBody &otherBody, const GXVec3 &otherConnectionPointLocal, GXFloat hardness, GXFloat restLength )
{
	this->connectionPointLocal = connectionPointLocal;
	this->otherBody = &otherBody;
	this->otherConnectionPointLocal = otherConnectionPointLocal;
	this->hardness = hardness;
	this->restLength = restLength;
}

GXVoid GXSpring::UpdateForce ( GXRigidBody &body, GXFloat /*deltaTime*/ )
{
	GXVec3 a;
	body.TranslatePointToWorld ( a, connectionPointLocal );

	GXVec3 b;
	otherBody->TranslatePointToWorld ( b, otherConnectionPointLocal );

	GXVec3 delta;
	GXSubVec3Vec3 ( delta, a, b );

	GXFloat stress = fabsf ( GXLengthVec3 ( delta ) - restLength ) * hardness;
	GXNormalizeVec3 ( delta );

	GXVec3 force;
	GXMulVec3Scalar ( force, delta, -stress );

	body.AddForceAtPointWorld ( force, a );
}
