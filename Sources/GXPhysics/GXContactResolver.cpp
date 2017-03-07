//version 1.0

#include <GXPhysics/GXContactResolver.h>





GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
{
	if ( numContacts == 0 ) return;

	//TODO
}

GXVoid GXContactResolver::PrepareContacts ( GXContact* contactArray, GXUInt numContacts, GXFloat deltaTime )
{
	for ( GXUInt i = 0; i < numContacts; i++ )
		contactArray[ i ].CalculateInternals ( deltaTime );
}

GXVoid GXContactResolver::AdjustPositions ( GXContact* contacts, GXUInt numContacts, GXFloat deltaTime )
{
	GXUInt i;
	GXUInt index;
	GXVec3 linearChange[ 2 ];
	GXVec3 angularChange[ 2 ];

	GXFloat max;
	GXVec3 deltaPosition;

	positionIterationsUsed = 0;
	while ( positionIterationsUsed < positionIterations )
	{
		max = positionEpsilon;
		index = numContacts;

		for ( i = 0; i < numContacts; i++ )
		{
			GXFloat penetration = contacts[ i ].GetPenetration ();
			if ( contacts[ i ].GetPenetration () > max )
			{
				max = penetration;
				index = i;
			}
		}

		if ( index == numContacts ) break;

		contacts[ index ].MatchAwakeState ();

		// TODO
	}
}
