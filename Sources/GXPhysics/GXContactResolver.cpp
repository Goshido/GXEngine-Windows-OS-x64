// version 1.4

#include <GXPhysics/GXContactResolver.h>
#include <GXPhysics/GXContactFrictionlessConstraint.h>
#include <GXPhysics/GXContactTangentConstraint.h>
#include <GXPhysics/GXContactBitangentConstraint.h>


#define SOLVER_ITERATIONS               10u
#define DEFAULT_BAUMGARTE_FACTOR        0.5f
#define DEFAULT_STATIONARY_THRESHOLD    1.0e-4f

//---------------------------------------------------------------------------------------------------------------------

GXContactResolver::GXContactResolver ():
    pairBodyConstraintSolver ( SOLVER_ITERATIONS ),
    singleBodyConstraintSolver ( SOLVER_ITERATIONS ),
    baumgarteFactor ( DEFAULT_BAUMGARTE_FACTOR )
{
    SetStationaryThreshold ( DEFAULT_STATIONARY_THRESHOLD );
}

GXContactResolver::~GXContactResolver ()
{
    // NOTHING
}

GXFloat GXContactResolver::GetBaumgarteFactor () const
{
    return baumgarteFactor;
}

GXVoid GXContactResolver::SetBaumgarteFactor ( GXFloat factor )
{
    baumgarteFactor = factor;
}

GXFloat GXContactResolver::GetStationaryThreshold () const
{
    return stationaryThreshold;
}

GXVoid GXContactResolver::SetStationaryThreshold ( GXFloat threshold )
{
    stationaryThreshold = threshold;
    squareStationaryThreshold = threshold * threshold;
}

GXVoid GXContactResolver::ResolveContacts ( GXContact* contactArray, GXUInt numContacts )
{
    GXUInt i = 0u;

    while ( i < numContacts )
    {
        GXContact& contact = contactArray[ i ];
        GXRigidBody& secondRigidBody = contact.GetSecondRigidBody ();

        if ( secondRigidBody.IsKinematic () )
            ResolveSingleBodyContacts ( contactArray + i );
        else
            ResolvePairBodyContacts ( contactArray + i );

        i += contact.GetLinkedContacts ();
    }
}

GXVoid GXContactResolver::ResolvePairBodyContacts ( GXContact* contacts )
{
    const GXUInt linkedContacts = contacts->GetLinkedContacts ();
    const GXFloat inverseLinkedContacts = 1.0f / static_cast<GXFloat> ( linkedContacts );

    GXRigidBody& firstBody = contacts->GetFirstRigidBody ();
    GXRigidBody& secondBody = contacts->GetSecondRigidBody ();

    GXVec3 averageNormal ( 0.0f, 0.0f, 0.0f );

    pairBodyConstraintSolver.Begin ( firstBody, secondBody );

    for ( GXUInt i = 0u; i < linkedContacts; ++i )
    {
        pairBodyConstraintSolver.AddConstraint ( GXContactFrictionlessConstraint ( contacts[ i ], baumgarteFactor ) );
        pairBodyConstraintSolver.AddConstraint ( GXContactTangentConstraint ( contacts[ i ], inverseLinkedContacts, squareStationaryThreshold ) );
        pairBodyConstraintSolver.AddConstraint ( GXContactBitangentConstraint ( contacts[ i ], inverseLinkedContacts, squareStationaryThreshold ) );
        
        averageNormal.Sum ( averageNormal, contacts[ i ].GetNormal () );
        averageNormal.Normalize ();
    }

    pairBodyConstraintSolver.End ();

    GXVec3 pushFactor;
    pushFactor.Multiply ( averageNormal, contacts->GetPenetration () * averageNormal.DotProduct ( contacts->GetNormal () ) );

    GXFloat alpha = firstBody.GetMass () / ( firstBody.GetMass () + secondBody.GetMass () );
    
    GXVec3 betta;
    betta.Sum ( firstBody.GetLocation (), alpha, pushFactor );
    firstBody.SetLocation ( betta );

    betta.Sum ( secondBody.GetLocation (), alpha - 1.0f, pushFactor );
}

GXVoid GXContactResolver::ResolveSingleBodyContacts ( GXContact* contacts )
{
    const GXUInt linkedContacts = contacts->GetLinkedContacts ();
    const GXFloat inverseLinkedContacts = 1.0f / static_cast<GXFloat> ( linkedContacts );

    GXRigidBody& firstBody = contacts->GetFirstRigidBody ();
    GXRigidBody& secondBody = contacts->GetSecondRigidBody ();

    GXVec3 averageNormal ( 0.0f, 0.0f, 0.0f );

    singleBodyConstraintSolver.Begin ( firstBody, secondBody );

    for ( GXUInt i = 0u; i < linkedContacts; ++i )
    {
        singleBodyConstraintSolver.AddConstraint ( GXContactFrictionlessConstraint ( contacts[ i ], baumgarteFactor ) );
        singleBodyConstraintSolver.AddConstraint ( GXContactTangentConstraint ( contacts[ i ], inverseLinkedContacts, squareStationaryThreshold ) );
        singleBodyConstraintSolver.AddConstraint ( GXContactBitangentConstraint ( contacts[ i ], inverseLinkedContacts, squareStationaryThreshold ) );
        
        averageNormal.Sum ( averageNormal, contacts[ i ].GetNormal () );
        averageNormal.Normalize ();
    }

    singleBodyConstraintSolver.End ();

    GXVec3 pushFactor;
    pushFactor.Multiply ( averageNormal, contacts->GetPenetration () * averageNormal.DotProduct ( contacts->GetNormal () ) );

    GXVec3 newLocation;
    newLocation.Sum ( firstBody.GetLocation (), pushFactor );

    firstBody.SetLocation ( newLocation );
}
