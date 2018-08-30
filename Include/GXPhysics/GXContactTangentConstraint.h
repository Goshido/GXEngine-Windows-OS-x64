// version 1.2

#ifndef GX_CONTACT_TANGENT_CONSTRAINT
#define GX_CONTACT_TANGENT_CONSTRAINT


#include <GXPhysics/GXConstraint.h>
#include <GXPhysics/GXContact.h>


// Implementation is based on paper
// "Iterative Dynamics with Temporal Coherence"
// by Erin Catto
// It is used friction approximation which is not physics correct but
// gives plausible result.

class GXContactTangentConstraint final : public GXConstraint
{
	public:
		// inverseLinkedContacts - is 1.0f / ( total manifold contacts ).
		// squareThreshold - is square magnitude of relative contact point velocity projection onto tangent/bitangent plane.
		// If this magnitude is less than squareThreshold contact point is considered to be stationary and will be
		// used coefficient of static friction. Otherwise will be used coefficient of dynamic friction.
		explicit GXContactTangentConstraint ( GXContact &contact, GXFloat inverseLinkedContacts, GXFloat squareThreshold );

		~GXContactTangentConstraint ();

	private:
		GXContactTangentConstraint ( const GXContactTangentConstraint &other ) = delete;
		GXContactTangentConstraint& operator = ( const GXContactTangentConstraint &other ) = delete;
};


#endif // GX_CONTACT_TANGENT_CONSTRAINT
