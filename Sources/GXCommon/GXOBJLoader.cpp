//version 1.5

#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>


GXInt	gx_objloader_Index = 0;
GXChar* gx_objloader_ObjSource = 0;
GXUInt	gx_objloader_FileSize = 0;
GXChar	gx_objloader_T[ 30 ] = { 0 };

GXUInt  gx_objloader_TotalVertexes = 0;
GXUInt  gx_objloader_TotalNormals = 0;
GXUInt  gx_objloader_TotalUVs = 0;
GXUInt  gx_objloader_TotalTriangles = 0;


#define SKIP_COMMENT()																												\
	while ( gx_objloader_ObjSource[ gx_objloader_Index ] != '\n' && ( gx_objloader_Index < (GXInt)gx_objloader_FileSize - 1 ) )		\
		gx_objloader_Index++;																										\
	gx_objloader_Index++

#define SKIP_GAPS()																													\
	while ( gx_objloader_ObjSource[ gx_objloader_Index ] == ' ' && ( gx_objloader_Index < (GXInt)gx_objloader_FileSize - 1 ) )		\
		gx_objloader_Index++

#define NEW_LINE_SOLVER()																																									\
	while ( gx_objloader_ObjSource[ gx_objloader_Index ] != 'v' && gx_objloader_ObjSource[ gx_objloader_Index ] != 'f' && ( gx_objloader_Index < (GXInt)gx_objloader_FileSize - 1 ) )		\
	{																																														\
		switch ( gx_objloader_ObjSource[ gx_objloader_Index ] )																																\
		{																																													\
			case '#':																																										\
				SKIP_COMMENT ();																																							\
			break;																																											\
																																															\
			case '\n':																																										\
				gx_objloader_Index++;																																						\
			break;																																											\
																																															\
			default:																																										\
				SKIP_COMMENT ();																																							\
			break;																																											\
		}																																													\
		SKIP_GAPS ();																																										\
	}									


template <class Pointer > 
GXVoid OBJListDelete ( Pointer top )
{
	Pointer p = top;
	while ( top != 0 )
	{
		top = top->next;
		delete p;
		p = top;
	}
}

GXOBJVertex* GXCALL GXParseVertexes ()
{
	#ifdef DEBUG_TO_LOG
		GXLogA ( "GXParseVertexes::Info - Parsing Vertexes\n" );
	#endif

	GXOBJVertex *vTop;
	GXOBJVertex *vTek;
	GXInt i = 0;

	vTop = new GXOBJVertex;
	vTek = vTop;
	vTek->next = 0;

	while ( GX_TRUE )
	{
		vTek->next = new GXOBJVertex;
		vTek = vTek->next;
		vTek->next = 0;

		gx_objloader_TotalVertexes++;

		SKIP_GAPS ();
		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != ' ' )
		{ 
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];  
			gx_objloader_Index++; 
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		vTek->position.x = (GXFloat)atof ( gx_objloader_T );

		SKIP_GAPS ();
		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != ' ' )
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];  
			gx_objloader_Index++; 
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		vTek->position.y = (GXFloat)atof ( gx_objloader_T );

		SKIP_GAPS ();
		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != '\n' && gx_objloader_ObjSource[ gx_objloader_Index ] != ' ' )
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];  
			gx_objloader_Index++; 
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		vTek->position.z = (GXFloat)atof ( gx_objloader_T );

		SKIP_COMMENT ();
		SKIP_GAPS ();
		NEW_LINE_SOLVER ();
		gx_objloader_Index++;
		if ( gx_objloader_ObjSource[ gx_objloader_Index ] != ' ' ) break;
	}
	
	vTek = vTop; 
	vTop = vTop->next; 
	delete vTek;

	#ifdef DEBUG_TO_LOG
		GXLogA ( "GXParseVertexes::Info - Done\n" );
	#endif

	return vTop;
}


GXOBJNormals* GXCALL GXParseNormals ()
{
	#ifdef DEBUG_TO_LOG
		GXLogA ( "GXParseNormals::Info - Parsing Normals\n" );
	#endif

	GXOBJNormals* nTop = new GXOBJNormals;
	GXOBJNormals* nTek = nTop;
	nTek->next = 0;
	GXInt i = 0;

	gx_objloader_Index++;

	while ( GX_TRUE )
	{
		SKIP_GAPS ();
		nTek->next = new GXOBJNormals;
		nTek = nTek->next;
		nTek->next = 0;

		gx_objloader_TotalNormals++;

		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != ' ')
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];
			gx_objloader_Index++;
			i++;
		}
		gx_objloader_T[i] = '\0';
		nTek->normal.x = (GXFloat)atof ( gx_objloader_T );

		SKIP_GAPS ();
		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != ' ')
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];
			gx_objloader_Index++;
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		nTek->normal.y = (GXFloat)atof ( gx_objloader_T );

		SKIP_GAPS ();
		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != '\n' && gx_objloader_ObjSource[ gx_objloader_Index ] != ' ')
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];
			gx_objloader_Index++;
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		nTek->normal.z = (GXFloat)atof ( gx_objloader_T );

		SKIP_COMMENT ();
		SKIP_GAPS ();
		NEW_LINE_SOLVER ();
		gx_objloader_Index++;
		if ( gx_objloader_ObjSource[ gx_objloader_Index ] != 'n' ) break;
		gx_objloader_Index++;
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

	GXOBJUV_s* uvTop = new GXOBJUV_s;
	GXOBJUV_s* uvTek = uvTop;
	uvTek->next = 0;
	GXInt i = 0;
	gx_objloader_Index++;

	while ( GX_TRUE )
	{
		SKIP_GAPS ();
		uvTek->next = new GXOBJUV_s;
		uvTek = uvTek->next;
		uvTek->next = 0;

		gx_objloader_TotalUVs++;

		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != ' ')
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];
			gx_objloader_Index++;
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		uvTek->uv.u = (GXFloat)atof ( gx_objloader_T );

		SKIP_GAPS ();
		i = 0;
		while ( gx_objloader_ObjSource[ gx_objloader_Index ] != '\n' && gx_objloader_ObjSource[ gx_objloader_Index ] != ' ' )
		{
			gx_objloader_T[ i ] = gx_objloader_ObjSource[ gx_objloader_Index ];
			gx_objloader_Index++;
			i++;
		}
		gx_objloader_T[ i ] = '\0';
		uvTek->uv.v = (GXFloat)atof ( gx_objloader_T );

		SKIP_COMMENT ();
		SKIP_GAPS ();
		NEW_LINE_SOLVER ();
		gx_objloader_Index++;
		if ( gx_objloader_ObjSource[ gx_objloader_Index ] != 't' ) break;
		gx_objloader_Index++;
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

	GXOBJTriangle* tgTop = new GXOBJTriangle;
	GXOBJTriangle* tgTek = tgTop;
	tgTek->next = 0;
	GXInt i = 0;

	while ( GX_TRUE )
	{
		tgTek->next = new GXOBJTriangle;
		tgTek = tgTek->next;
		tgTek->next = 0;
		
		gx_objloader_TotalTriangles++;

		for ( GXInt j = 0; j < 3; j++ )
		{
			SKIP_GAPS ();
			i = 0;
			while ( gx_objloader_ObjSource [ gx_objloader_Index ] != '/')
			{
				gx_objloader_T[ i ] = gx_objloader_ObjSource [ gx_objloader_Index ];
				gx_objloader_Index++;
				i++;
			}
			gx_objloader_T[ i ] = '\0';
			tgTek->dat[ j ].v = atoi ( gx_objloader_T ); 

			gx_objloader_Index++;
			SKIP_GAPS ();
			i = 0;
			while ( gx_objloader_ObjSource [ gx_objloader_Index ] != '/')
			{
				gx_objloader_T[ i ] = gx_objloader_ObjSource [ gx_objloader_Index ];
				gx_objloader_Index++;
				i++;
			}
			gx_objloader_T[ i ] = '\0';
			tgTek->dat[ j ].vt = atoi ( gx_objloader_T );

			gx_objloader_Index++;
			SKIP_GAPS ();
			i = 0;
			while ( gx_objloader_ObjSource [ gx_objloader_Index ] != '\n' && gx_objloader_ObjSource [ gx_objloader_Index ] != ' ' )
			{
				gx_objloader_T[ i ] = gx_objloader_ObjSource [ gx_objloader_Index ];
				gx_objloader_Index++;
				i++;
			}
			gx_objloader_T[ i ] = '\0';
			tgTek->dat[ j ].vn = atoi ( gx_objloader_T );
		}

		SKIP_COMMENT ();
		SKIP_GAPS ();
		NEW_LINE_SOLVER ();
		if ( gx_objloader_Index > ( GXInt )gx_objloader_FileSize - 1 ) break;
		gx_objloader_Index++;
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
	if ( !GXLoadFile ( fileName, (GXVoid**)&gx_objloader_ObjSource, gx_objloader_FileSize, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadOBJ::Error - Не могу загрузит файл %s\n", fileName );
		return 0;
	}

	setlocale ( LC_ALL, "C" );

	gx_objloader_TotalVertexes = 0;
	gx_objloader_TotalNormals = 0;
	gx_objloader_TotalUVs = 0;
	gx_objloader_TotalTriangles = 0;

	GXOBJVertex *vTop = 0;
	GXOBJNormals *nTop = 0;
	GXOBJUV_s *uvTop = 0;

	GXOBJTriangle *tgTop = 0;
	GXOBJTriangle *tgTek = 0;

	gx_objloader_Index = 0;

	NEW_LINE_SOLVER ();
	gx_objloader_Index++;
	vTop = GXParseVertexes ();

	switch ( gx_objloader_ObjSource[ gx_objloader_Index ] )
	{
		case 'n':
			nTop = GXParseNormals ();
		break;

		case 't':
			uvTop = GXParseUVs ();
		break;
	}

	switch ( gx_objloader_ObjSource[ gx_objloader_Index ] )
	{
		case 'n':
			nTop = GXParseNormals ();
		break;

		case 't':
			uvTop = GXParseUVs ();
		break;
	}

	gx_objloader_Index++;
	tgTop = GXParseTriangles ();
	GXSafeFree ( gx_objloader_ObjSource );

	#ifdef DEBUG_TO_LOG
		GXLogA ( "GXLoadOBJ::Info - Forming structure. %i vertexes, %i normals, %i uv_s, %i triangles\n", gx_objloader_TotalVertexes, gx_objloader_TotalNormals, gx_objloader_TotalUVs, gx_objloader_TotalTriangles );
	#endif

	GXVec3* vertexes = (GXVec3*)malloc ( gx_objloader_TotalVertexes * sizeof ( GXVec3 ) );
	GXOBJVertex* t = vTop;
	for ( GXUInt i = 0; i < gx_objloader_TotalVertexes; i++, t = t->next )
		vertexes[ i ] = t->position;
	OBJListDelete ( vTop );

	GXVec3* normals = (GXVec3*)malloc ( gx_objloader_TotalNormals * sizeof ( GXVec3 ) );
	GXOBJNormals* n = nTop;
	for ( GXUInt i = 0; i < gx_objloader_TotalNormals; i++, n = n->next )
		normals[ i ] = n->normal;
	OBJListDelete ( nTop );

	GXVec2* uv_s = (GXVec2*)malloc ( gx_objloader_TotalUVs * sizeof ( GXVec2 ) );
	GXOBJUV_s* u = uvTop;
	for ( GXUInt i = 0; i < gx_objloader_TotalUVs; i++, u = u->next )
		uv_s[ i ] = u->uv;
	OBJListDelete ( uvTop );

	*points = (GXOBJPoint*)malloc ( 3 * gx_objloader_TotalTriangles * sizeof ( GXOBJPoint ) );
	GXOBJPoint* outPoints = *points;

	tgTek = tgTop;
	GXInt totalElements = 0;
	while ( tgTek != 0 )
	{
		for ( GXUChar j = 0; j < 3; j++ )
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

			totalElements++;
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
