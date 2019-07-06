// version 1.2

#ifndef GX_CONTACT_GENERATOR
#define GX_CONTACT_GENERATOR


#include "GXCollisionData.h"


class GXContactGenerator
{
    public:
        GXContactGenerator ();
        virtual ~GXContactGenerator ();

        virtual GXUInt AddContact ( GXCollisionData &collisionData ) const = 0;

    private:
        GXContactGenerator ( const GXContactGenerator &other ) = delete;
        GXContactGenerator& operator = ( const GXContactGenerator &other ) = delete;
};


#endif // GX_CONTACT_GENERATOR
