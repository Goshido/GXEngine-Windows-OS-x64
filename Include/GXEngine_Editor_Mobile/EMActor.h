#ifndef EM_ACTOR
#define EM_ACTOR


#include <GXEngine/GXTransform.h>
#include <GXCommon/GXMath.h>


class EMActor;
extern EMActor* em_Actors;


enum class eEMActorType : GXUInt
{
    Abstact = 0,
    DirectedLight = 1,
    UnitCube = 2,
    Mesh = 3,
    PhysicsDrivenActor = 4
};

struct EMActorHeader
{
    GXUInt          _type;
    GXUBigInt       _nameOffset;
    GXMat4          _origin;
    GXBool          _isVisible;
    GXUBigInt       _size;
};

class EMActor
{
    private:
        EMActor*        _next;
        EMActor*        _previous;

    protected:
        eEMActorType    _type;
        GXTransform     _transform;
        GXBool          _isVisible;
        GXWChar*        _name;

    public:
        explicit EMActor ( const GXWChar* name, eEMActorType type, const GXTransform &transform );
        virtual ~EMActor ();

        virtual GXVoid OnDrawCommonPass ( GXFloat deltaTime );
        virtual GXVoid OnDrawHudColorPass ();
        virtual GXVoid OnDrawHudMaskPass ();
        
        virtual GXVoid OnUpdate ( GXFloat deltaTime );
        virtual GXVoid OnSave ( GXUByte** data );
        virtual GXVoid OnLoad ( const GXUByte* data );
        virtual GXUPointer OnRequeredSaveSize () const;
        virtual GXVoid OnTransformChanged ();

        const GXWChar* GetName ();
        eEMActorType GetType ();

        const GXTransform& GetTransform ();
        GXVoid SetTransform ( const GXTransform &newTransform );

        GXVoid Show ();
        GXVoid Hide ();

    private:
        EMActor () = delete;
        EMActor ( const EMActor &other ) = delete;
        EMActor& operator = ( const EMActor &other ) = delete;
};


#endif // EM_ACTOR
