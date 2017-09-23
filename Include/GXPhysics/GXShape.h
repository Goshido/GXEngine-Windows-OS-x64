//version 1.0

#ifndef GX_SHAPE
#define GX_SHAPE


#include <GXCommon/GXMath.h>


enum class eGXShapeType
{
	Sphere,
	Box,
	Plane,
	Polygon
};

class GXRigidBody;
class GXShape
{
	protected:
		eGXShapeType	type;
		GXRigidBody*	body;
		GXMat4			transformRigidBody;
		GXMat4			transformWorld;
		GXMat3			inertialTensor;

		GXFloat			staticFriction;
		GXFloat			dynamicFriction;
		GXFloat			restitution;

	public:
		GXShape ( eGXShapeType type, GXRigidBody* body );
		virtual ~GXShape ();

		eGXShapeType GetType () const;

		const GXMat4& GetTransformWorld () const;
		GXVoid CalculateCacheData ();
		GXRigidBody& GetRigidBody () const;

		virtual GXVoid CalculateInertiaTensor ( GXFloat mass ) = 0;
		const GXMat3& GetInertialTensor () const;

		virtual GXVoid GetExtremePoint ( GXVec3 &point, const GXVec3 &direction ) const = 0;

		GXVoid SetFriction ( GXFloat newStaticFriction, GXFloat newDynamicFriction );
		GXFloat GetStaticFriction () const;
		GXFloat GetDynamicFriction () const;

		GXVoid SetRestitution ( GXFloat newRestitution );
		GXFloat GetRestitution () const;
};


#endif GX_SHAPE
