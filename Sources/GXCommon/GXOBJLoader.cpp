// version 1.8

#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


static GXInt            gx_objLoaderIndex = 0;
static GXChar*          gx_objLoaderObjSource = nullptr;
static GXUPointer       gx_objLoaderFileSize = 0u;
static GXChar           gx_objLoaderWorkingArray[ 30u ] = { 0 };

static GXUInt           gx_objLoaderTotalVertices = 0u;
static GXUInt           gx_objLoaderTotalNormals = 0u;
static GXUInt           gx_objLoaderTotalUVs = 0u;
static GXUInt           gx_objLoaderTotalTriangles = 0u;


#define SKIP_COMMENT()                                                                                                          \
    while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != '\n' && ( gx_objLoaderIndex < (GXInt)gx_objLoaderFileSize - 1 ) )     \
        ++gx_objLoaderIndex;                                                                                                    \
    ++gx_objLoaderIndex

#define SKIP_GAPS()                                                                                                             \
    while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] == ' ' && ( gx_objLoaderIndex < (GXInt)gx_objLoaderFileSize - 1 ) )      \
        ++gx_objLoaderIndex

#define NEW_LINE_SOLVER()                                                                                                                                                           \
    while ( gx_objLoaderObjSource[ gx_objLoaderIndex ] != 'v' && gx_objLoaderObjSource[ gx_objLoaderIndex ] != 'f' && ( gx_objLoaderIndex < (GXInt)gx_objLoaderFileSize - 1 ) )     \
    {                                                                                                                                                                               \
        switch ( gx_objLoaderObjSource[ gx_objLoaderIndex ] )                                                                                                                       \
        {                                                                                                                                                                           \
            case '#':                                                                                                                                                               \
                SKIP_COMMENT ();                                                                                                                                                    \
            break;                                                                                                                                                                  \
                                                                                                                                                                                    \
            case '\n':                                                                                                                                                              \
                ++gx_objLoaderIndex;                                                                                                                                                \
            break;                                                                                                                                                                  \
                                                                                                                                                                                    \
            default:                                                                                                                                                                \
                SKIP_COMMENT ();                                                                                                                                                    \
            break;                                                                                                                                                                  \
        }                                                                                                                                                                           \
        SKIP_GAPS ();                                                                                                                                                               \
    }                                    


template <class Pointer > 
GXVoid OBJListDelete ( Pointer top )
{
    while ( !top )
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

    vTop = new GXOBJVertex ();
    vTek = vTop;
    vTek->next = nullptr;

    while ( GX_TRUE )
    {
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

    GXOBJNormals* nTop = new GXOBJNormals ();
    GXOBJNormals* nTek = nTop;
    nTek->next = nullptr;
    GXInt i = 0;

    ++gx_objLoaderIndex;

    while ( GX_TRUE )
    {
        SKIP_GAPS ();
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

    GXOBJUV_s* uvTop = new GXOBJUV_s ();
    GXOBJUV_s* uvTek = uvTop;
    uvTek->next = nullptr;
    GXInt i = 0;
    ++gx_objLoaderIndex;

    while ( GX_TRUE )
    {
        SKIP_GAPS ();

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

    GXOBJTriangle* tgTop = new GXOBJTriangle ();
    GXOBJTriangle* tgTek = tgTop;
    tgTek->next = nullptr;
    GXInt i = 0;

    while ( GX_TRUE )
    {
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

GXInt GXCALL GXLoadOBJ ( const GXWChar* fileName, GXOBJPoint** points )
{
    if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &gx_objLoaderObjSource ), gx_objLoaderFileSize, GX_TRUE ) )
    {
        GXLogW ( L"GXLoadOBJ::Error - Не могу загрузить файл %s\n", fileName );
        return 0;
    }

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
    GXSafeFree ( gx_objLoaderObjSource );

    #ifdef DEBUG_TO_LOG
        GXLogA ( "GXLoadOBJ::Info - Forming structure. %i vertexes, %i normals, %i uv_s, %i triangles\n", gx_objLoaderTotalVertices, gx_objLoaderTotalNormals, gx_objLoaderTotalUVs, gx_objLoaderTotalTriangles );
    #endif

    GXVec3* vertexes = static_cast<GXVec3*> ( malloc ( gx_objLoaderTotalVertices * sizeof ( GXVec3 ) ) );
    GXOBJVertex* t = vTop;

    for ( GXUInt i = 0u; i < gx_objLoaderTotalVertices; ++i, t = t->next )
        vertexes[ i ] = t->position;

    OBJListDelete ( vTop );

    GXVec3* normals = static_cast<GXVec3*> ( malloc ( gx_objLoaderTotalNormals * sizeof ( GXVec3 ) ) );
    GXOBJNormals* n = nTop;

    for ( GXUInt i = 0u; i < gx_objLoaderTotalNormals; ++i, n = n->next )
        normals[ i ] = n->normal;

    OBJListDelete ( nTop );

    GXVec2* uv_s = static_cast<GXVec2*> ( malloc ( gx_objLoaderTotalUVs * sizeof ( GXVec2 ) ) );
    GXOBJUV_s* u = uvTop;

    for ( GXUInt i = 0u; i < gx_objLoaderTotalUVs; ++i, u = u->next )
        uv_s[ i ] = u->uv;

    OBJListDelete ( uvTop );

    *points = static_cast<GXOBJPoint*> ( malloc ( 3 * gx_objLoaderTotalTriangles * sizeof ( GXOBJPoint ) ) );
    GXOBJPoint* outPoints = *points;

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

    free ( vertexes );
    free ( normals );
    free ( uv_s );

    OBJListDelete ( tgTop );

    return totalElements;
}
