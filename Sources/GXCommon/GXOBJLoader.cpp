// version 1.9

#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


static GXInt            gx_objLoaderIndex = 0;
static GXChar*          gx_objLoaderObjSource = nullptr;
static GXUPointer       gx_objLoaderFileSize = 0u;
static GXChar           gx_objLoaderWorkingArray[ 30u ] = { 0 };

static GXUInt           gx_objLoaderTotalVertices = 0u;
static GXUInt           gx_objLoaderTotalNormals = 0u;
static GXUInt           gx_objLoaderTotalUVs = 0u;
static GXUInt           gx_objLoaderTotalTriangles = 0u;


#define SKIP_COMMENT()                                                                                                                          \
    while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != '\n' && ( gx_objLoaderIndex < static_cast<GXInt> ( gx_objLoaderFileSize ) - 1 ) )     \
        ++gx_objLoaderIndex;                                                                                                                    \
    ++gx_objLoaderIndex

#define SKIP_GAPS()                                                                                                                             \
    while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] == ' ' && ( gx_objLoaderIndex < static_cast<GXInt> ( gx_objLoaderFileSize ) - 1 ) )      \
        ++gx_objLoaderIndex

#define NEW_LINE_SOLVER()                                                                                                                                                                           \
    while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != 'v' && gx_objLoaderObjSource[ gx_objLoaderIndex ] != 'f' && ( gx_objLoaderIndex < static_cast<GXInt> ( gx_objLoaderFileSize ) - 1 ) )     \
    {                                                                                                                                                                                               \
        switch ( gx_objLoaderObjSource[ gx_objLoaderIndex ] )                                                                                                                                       \
        {                                                                                                                                                                                           \
            case '#':                                                                                                                                                                               \
                SKIP_COMMENT ();                                                                                                                                                                    \
            break;                                                                                                                                                                                  \
                                                                                                                                                                                                    \
            case '\n':                                                                                                                                                                              \
                ++gx_objLoaderIndex;                                                                                                                                                                \
            break;                                                                                                                                                                                  \
                                                                                                                                                                                                    \
            default:                                                                                                                                                                                \
                SKIP_COMMENT ();                                                                                                                                                                    \
            break;                                                                                                                                                                                  \
        }                                                                                                                                                                                           \
        SKIP_GAPS ();                                                                                                                                                                               \
    }

//---------------------------------------------------------------------------------------------------------------------

class GXOBJLoaderMemoryInspector final : public GXMemoryInspector
{
    public:
        GXOBJLoaderMemoryInspector ();
        ~GXOBJLoaderMemoryInspector () override;

    private:
        GXOBJLoaderMemoryInspector ( const GXOBJLoaderMemoryInspector &other );
        GXOBJLoaderMemoryInspector& operator = ( const GXOBJLoaderMemoryInspector &other );
};

GXOBJLoaderMemoryInspector::GXOBJLoaderMemoryInspector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXOBJLoaderMemoryInspector" )
{
    // NOTHING
}

GXOBJLoaderMemoryInspector::~GXOBJLoaderMemoryInspector ()
{
    // NOTHING
}

static GXOBJLoaderMemoryInspector gx_OBJLoaderMemoryInspector;

//---------------------------------------------------------------------------------------------------------------------

struct GXOBJIndex final
{
    GXInt       v;
    GXInt       vn;
    GXInt       vt;
};

//---------------------------------------------------------------------------------------------------------------------

class GXOBJVertex final : public GXMemoryInspector
{
    public:
        GXVec3              position;
        GXOBJVertex*        next;

    public:
        GXOBJVertex ();
        ~GXOBJVertex () override;

    private:
        GXOBJVertex ( const GXOBJVertex &other ) = delete;
        GXOBJVertex& operator = ( const GXOBJVertex &other ) = delete;
};

GXOBJVertex::GXOBJVertex ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXOBJVertex" )
{
    // NOTHING
}

GXOBJVertex::~GXOBJVertex ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXOBJNormals final : public GXMemoryInspector
{
    public:
        GXVec3              normal;
        GXOBJNormals*       next;

    public:
        GXOBJNormals ();
        ~GXOBJNormals () override;

    private:
        GXOBJNormals ( const GXOBJNormals &other ) = delete;
        GXOBJNormals& operator = ( const GXOBJNormals &other ) = delete;
};

GXOBJNormals::GXOBJNormals ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXOBJNormals" )
{
    // NOTHING
}

GXOBJNormals::~GXOBJNormals ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXOBJUV_s final : public GXMemoryInspector
{
    public:
        GXVec2          uv;
        GXOBJUV_s*      next;

    public:
        GXOBJUV_s ();
        ~GXOBJUV_s () override;

    private:
        GXOBJUV_s ( const GXOBJUV_s &other ) = delete;
        GXOBJUV_s& operator = ( const GXOBJUV_s &other ) = delete;
};

GXOBJUV_s::GXOBJUV_s ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXOBJUV_s" )
{
    // NOTHING
}

GXOBJUV_s::~GXOBJUV_s ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

class GXOBJTriangle final : public GXMemoryInspector
{
    public:
        GXOBJIndex          dat[ 3u ];
        GXOBJTriangle*      next;

    public:
        GXOBJTriangle ();
        ~GXOBJTriangle () override;

    private:
        GXOBJTriangle ( const GXOBJTriangle &other ) = delete;
        GXOBJTriangle& operator = ( const GXOBJTriangle &other ) = delete;
};

GXOBJTriangle::GXOBJTriangle ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXOBJTriangle" )
{
    // NOTHING
}

GXOBJTriangle::~GXOBJTriangle ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

template<class Pointer>
GXVoid GXDeleteList ( Pointer top )
{
    while ( top )
    {
        Pointer p = top;
        top = top->next;
        delete p;
        p = top;
    }
}

GXOBJVertex* GXCALL GXParseVertices ()
{
    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseVertices::Info - Parsing Vertexes\n" );
    #endif

    GXOBJVertex *vTop;
    GXOBJVertex *vTek;
    GXInt i = 0;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJVertex" );
    vTop = new GXOBJVertex ();
    vTek = vTop;
    vTek->next = nullptr;

    for ( ; ; )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJVertex" );
        vTek->next = new GXOBJVertex ();
        vTek = vTek->next;
        vTek->next = nullptr;

        ++gx_objLoaderTotalVertices;

        SKIP_GAPS ();
        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' )
        { 
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];  
            ++gx_objLoaderIndex; 
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        vTek->position.SetX ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_GAPS ();
        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' )
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];  
            ++gx_objLoaderIndex; 
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        vTek->position.SetY ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_GAPS ();
        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != '\n' && gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' )
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];  
            ++gx_objLoaderIndex; 
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        vTek->position.SetZ ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_COMMENT ();
        SKIP_GAPS ();
        NEW_LINE_SOLVER ();

        ++gx_objLoaderIndex;

        if ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' ) break;
    }
    
    vTek = vTop; 
    vTop = vTop->next; 
    delete vTek;

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseVertices::Info - Done\n" );
    #endif

    return vTop;
}

GXOBJNormals* GXCALL GXParseNormals ()
{
    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseNormals::Info - Parsing Normals\n" );
    #endif

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJNormals" );
    GXOBJNormals* nTop = new GXOBJNormals ();
    GXOBJNormals* nTek = nTop;
    nTek->next = nullptr;
    GXInt i = 0;

    ++gx_objLoaderIndex;

    for ( ; ; )
    {
        SKIP_GAPS ();

        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJNormals" );
        nTek->next = new GXOBJNormals ();
        nTek = nTek->next;
        nTek->next = nullptr;

        ++gx_objLoaderTotalNormals;

        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ')
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];
            ++gx_objLoaderIndex;
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        nTek->normal.SetX ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_GAPS ();
        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ')
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];
            ++gx_objLoaderIndex;
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        nTek->normal.SetY ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_GAPS ();
        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != '\n' && gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' )
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];
            ++gx_objLoaderIndex;
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        nTek->normal.SetZ ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_COMMENT ();
        SKIP_GAPS ();
        NEW_LINE_SOLVER ();

        ++gx_objLoaderIndex;

        if ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != 'n' ) break;

        ++gx_objLoaderIndex;
    }

    nTek = nTop; 
    nTop = nTop->next; 
    delete nTek;

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseNormals::Info - Done\n" );
    #endif

    return nTop;
}

GXOBJUV_s* GXCALL GXParseUVs ()
{
    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseUVs::Info - Parsing UV_s\n" );
    #endif

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJUV_s" );
    GXOBJUV_s* uvTop = new GXOBJUV_s ();
    GXOBJUV_s* uvTek = uvTop;
    uvTek->next = nullptr;
    GXInt i = 0;
    ++gx_objLoaderIndex;

    for ( ; ; )
    {
        SKIP_GAPS ();

        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJUV_s" );
        uvTek->next = new GXOBJUV_s ();
        uvTek = uvTek->next;
        uvTek->next = nullptr;

        ++gx_objLoaderTotalUVs;

        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' )
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];
            ++gx_objLoaderIndex;
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        uvTek->uv.SetX ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_GAPS ();
        i = 0;

        while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != '\n' && gx_objLoaderObjSource[ gx_objLoaderIndex ] != ' ' )
        {
            gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource[ gx_objLoaderIndex ];
            ++gx_objLoaderIndex;
            ++i;
        }

        gx_objLoaderWorkingArray[ i ] = '\0';
        uvTek->uv.SetY ( static_cast<GXFloat> ( atof ( gx_objLoaderWorkingArray ) ) );

        SKIP_COMMENT ();
        SKIP_GAPS ();
        NEW_LINE_SOLVER ();

        ++gx_objLoaderIndex;

        if ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != 't' ) break;

        ++gx_objLoaderIndex;
    }

    uvTek = uvTop; 
    uvTop = uvTop->next; 
    delete uvTek;

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseUVs::Info - Done\n" );
    #endif

    return uvTop;
}

GXOBJTriangle* GXCALL GXParseTriangles ()
{
    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseTriangles::Info - Parsing Triangles\n" );
    #endif

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJTriangle" );
    GXOBJTriangle* tgTop = new GXOBJTriangle ();
    GXOBJTriangle* tgTek = tgTop;
    tgTek->next = nullptr;
    GXInt i = 0;

    for ( ; ; )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXOBJTriangle" );
        tgTek->next = new GXOBJTriangle ();
        tgTek = tgTek->next;
        tgTek->next = nullptr;
        
        ++gx_objLoaderTotalTriangles;

        for ( GXInt j = 0; j < 3; ++j )
        {
            SKIP_GAPS ();
            i = 0;

            while ( gx_objLoaderObjSource [ gx_objLoaderIndex ] != '/')
            {
                gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource [ gx_objLoaderIndex ];
                ++gx_objLoaderIndex;
                ++i;
            }

            gx_objLoaderWorkingArray[ i ] = '\0';
            tgTek->dat[ j ].v = atoi ( gx_objLoaderWorkingArray ); 

            ++gx_objLoaderIndex;
            SKIP_GAPS ();
            i = 0;

            while ( gx_objLoaderObjSource [ gx_objLoaderIndex ] != '/')
            {
                gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource [ gx_objLoaderIndex ];
                ++gx_objLoaderIndex;
                ++i;
            }

            gx_objLoaderWorkingArray[ i ] = '\0';
            tgTek->dat[ j ].vt = atoi ( gx_objLoaderWorkingArray );

            ++gx_objLoaderIndex;
            SKIP_GAPS ();
            i = 0;

            while ( gx_objLoaderObjSource [ gx_objLoaderIndex ] != '\n' && gx_objLoaderObjSource [ gx_objLoaderIndex ] != ' ' )
            {
                gx_objLoaderWorkingArray[ i ] = gx_objLoaderObjSource [ gx_objLoaderIndex ];
                ++gx_objLoaderIndex;
                ++i;
            }

            gx_objLoaderWorkingArray[ i ] = '\0';
            tgTek->dat[ j ].vn = atoi ( gx_objLoaderWorkingArray );
        }

        SKIP_COMMENT ();
        SKIP_GAPS ();
        NEW_LINE_SOLVER ();

        if ( gx_objLoaderIndex > static_cast<GXInt> ( gx_objLoaderFileSize - 1u ) ) break;

        ++gx_objLoaderIndex;
    }

    tgTek = tgTop;
    tgTop = tgTop->next;
    delete tgTek;

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXParseTriangles::Info - Done\n" );
    #endif

    return tgTop;
}

GXInt GXCALL GXLoadOBJ ( const GXWChar* fileName, GXOBJPoint*& points )
{
    GXUByte* rawData;
    GXFile file ( fileName );

    if ( !file.LoadContent ( rawData, gx_objLoaderFileSize, eGXFileContentOwner::GXFile, GX_TRUE ) )
    {
        GXLogA ( "GXLoadOBJ::Error - �� ���� ��������� ���� %S\n", fileName );
        return 0;
    }

    gx_objLoaderObjSource = reinterpret_cast<GXChar*> ( rawData );

    setlocale ( LC_ALL, "C" );

    gx_objLoaderTotalVertices = 0u;
    gx_objLoaderTotalNormals = 0u;
    gx_objLoaderTotalUVs = 0u;
    gx_objLoaderTotalTriangles = 0u;

    GXOBJVertex* vTop = nullptr;
    GXOBJNormals* nTop = nullptr;
    GXOBJUV_s* uvTop = nullptr;

    GXOBJTriangle* tgTop = nullptr;
    GXOBJTriangle* tgTek = nullptr;

    gx_objLoaderIndex = 0;

    NEW_LINE_SOLVER ();

    ++gx_objLoaderIndex;
    vTop = GXParseVertices ();

    switch ( gx_objLoaderObjSource[ gx_objLoaderIndex ] )
    {
        case 'n':
            nTop = GXParseNormals ();
        break;

        case 't':
            uvTop = GXParseUVs ();
        break;

        default:
            // NOTHING
        break;
    }

    switch ( gx_objLoaderObjSource[ gx_objLoaderIndex ] )
    {
        case 'n':
            nTop = GXParseNormals ();
        break;

        case 't':
            uvTop = GXParseUVs ();
        break;

        default:
            // NOTHING
        break;
    }

    ++gx_objLoaderIndex;
    tgTop = GXParseTriangles ();

    file.Close ();

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXLoadOBJ::Info - Forming structure. %i vertexes, %i normals, %i uv_s, %i triangles\n", gx_objLoaderTotalVertices, gx_objLoaderTotalNormals, gx_objLoaderTotalUVs, gx_objLoaderTotalTriangles );
    #endif

    GXVec3* vertexes = static_cast<GXVec3*> ( gx_OBJLoaderMemoryInspector.Malloc ( gx_objLoaderTotalVertices * sizeof ( GXVec3 ) ) );
    GXOBJVertex* t = vTop;

    for ( GXUInt i = 0u; i < gx_objLoaderTotalVertices; ++i, t = t->next )
        vertexes[ i ] = t->position;

    GXDeleteList ( vTop );

    GXVec3* normals = static_cast<GXVec3*> ( gx_OBJLoaderMemoryInspector.Malloc ( gx_objLoaderTotalNormals * sizeof ( GXVec3 ) ) );
    GXOBJNormals* n = nTop;

    for ( GXUInt i = 0u; i < gx_objLoaderTotalNormals; ++i, n = n->next )
        normals[ i ] = n->normal;

    GXDeleteList ( nTop );

    GXVec2* uv_s = static_cast<GXVec2*> ( gx_OBJLoaderMemoryInspector.Malloc ( gx_objLoaderTotalUVs * sizeof ( GXVec2 ) ) );
    GXOBJUV_s* u = uvTop;

    for ( GXUInt i = 0u; i < gx_objLoaderTotalUVs; ++i, u = u->next )
        uv_s[ i ] = u->uv;

    GXDeleteList ( uvTop );

    points = static_cast<GXOBJPoint*> ( gx_OBJLoaderMemoryInspector.Malloc ( 3u * gx_objLoaderTotalTriangles * sizeof ( GXOBJPoint ) ) );
    GXOBJPoint* outPoints = points;

    tgTek = tgTop;
    GXInt totalElements = 0;

    while ( tgTek != 0 )
    {
        for ( GXUChar j = 0u; j < 3u; ++j )
        {
            #ifdef DEBUG_TO_LOG
            if ( ( totalElements % 100000 ) == 0 )
                GXLogA ( "GXLoadOBJ::Info - Processed %i points\n", totalElements );
            #endif

            GXInt offset = tgTek->dat[ j ].v - 1;
            outPoints[ totalElements ].vertex = vertexes[ offset ];

            offset = tgTek->dat[ j ].vt - 1;
            outPoints[ totalElements ].uv = uv_s[ offset ];

            offset = tgTek->dat[ j ].vn - 1;
            outPoints[ totalElements ].normal = normals[ offset ];

            ++totalElements;
        }

        tgTek = tgTek->next;
    }

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXLoadOBJ::Info - Processed %i points\n", totalElements );
    #endif

    gx_OBJLoaderMemoryInspector.Free ( vertexes );
    gx_OBJLoaderMemoryInspector.Free ( normals );
    gx_OBJLoaderMemoryInspector.Free ( uv_s );

    GXDeleteList ( tgTop );

    return totalElements;
}
