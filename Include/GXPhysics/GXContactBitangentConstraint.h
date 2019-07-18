// version 1.3

#ifndef GX_CONTACT_BITANGENT_CONSTRAINT
#define GX_CONTACT_BITANGENT_CONSTRAINT


#include <GXPhysics/GXConstraint.h>
#include <GXPhysics/GXContact.h>


// Implementation is based on paper
// "Iterative Dynamics with Temporal Coherence"
// by Erin Catto
// It is used friction approximation which is not physics correct but
// gives plausible result.

class GXContactBitangentConstraint final : public GXConstraint
{
    public:
        // inverseLinkedContacts - is 1.0f / ( total manifold contacts ).
        // squareThreshold - is square magnitude of relative contact point velocity projection onto tangent/bitangent plane.
        // If this magnitude is less than squareThreshold contact point is considered to be stationary and will be
        // used coefficient of static friction. Otherwise will be used coefficient of dynamic friction.
        explicit GXContactBitangentConstraint ( GXContact &contact, GXFloat inverseLinkedContacts, GXFloat squareThreshold );

        ~GXContactBitangentConstraint ();

    private:
        GXContactBitangentConstraint ( const GXContactBitangentConstraint &other ) = delete;
        GXContactBitangentConstraint& operator = ( const GXContactBitangentConstraint &other ) = delete;
};


#endif // GX_CONTACT_BITANGENT_CONSTRAINT
