//version 1.1

#include <GXPlugin_for_3ds_Max/GXMeshNormalHolder.h>


class GXVertexNormal
{
	public:
		Point3				normal;
		DWORD				smoothGroup;
		GXBool				isInited;
		GXVertexNormal*		next;

	public:
		GXVertexNormal ();
		explicit GXVertexNormal ( const Point3 &normal, DWORD smoothGroup );
		~GXVertexNormal ();

		GXVoid AddNormal ( const Point3 &newNormal, DWORD newSmoothGroup );
		const Point3& GetNormal ( DWORD lookUpSmoothGroup ) const;
		GXVoid Normalize ();
};

GXVertexNormal::GXVertexNormal ()
{
	normal = Point3 ( 0.0f, 0.0f, 0.0f );
	smoothGroup = 0;
	isInited = GX_FALSE;
	next = nullptr;
}

GXVertexNormal::GXVertexNormal ( const Point3 &normal, DWORD smoothGroup )
{
	this->normal = normal;
	this->smoothGroup = smoothGroup;
	isInited = GX_TRUE;
	next = nullptr;
}

GXVertexNormal::~GXVertexNormal ()
{
	//NOTHING
}

GXVoid GXVertexNormal::AddNormal ( const Point3 &newNormal, DWORD newSmoothGroup )
{
	if ( !( this->smoothGroup & newSmoothGroup ) && isInited )
	{
		if ( next ) 
			next->AddNormal ( newNormal, newSmoothGroup );
		else 
			next = new GXVertexNormal ( newNormal, newSmoothGroup );
	}
	else 
	{
		this->normal += newNormal;
		this->smoothGroup |= newSmoothGroup;
		isInited = GX_TRUE;
	}
}

const Point3& GXVertexNormal::GetNormal ( DWORD lookUpSmoothGroup ) const
{
	if ( this->smoothGroup & lookUpSmoothGroup || !next )
		return normal;
	else
		return next->GetNormal ( lookUpSmoothGroup );
}

GXVoid GXVertexNormal::Normalize ()
{
	GXVertexNormal* ptr = next;
	GXVertexNormal* prev = this;

	while ( ptr )
	{
		if ( prev->smoothGroup & ptr->smoothGroup  ) 
		{
			prev->normal += ptr->normal;
			prev->next = ptr->next;
			delete ptr;
			ptr = prev->next;
		}
		else 
		{
			prev = ptr;
			ptr = ptr->next;
		}
	}

	normal = ::Normalize ( normal );

	if ( next )
		next->Normalize ();
}

//--------------------------------------------------------------------------

GXMeshNormalHolder::GXMeshNormalHolder ( Mesh &mesh ):
mesh(mesh)
{
	//NOTHING
}

GXMeshNormalHolder::~GXMeshNormalHolder ()
{
	//NOTHING
}

GXVoid GXMeshNormalHolder::BuildNormals ()
{
	Face* face = mesh.faces; 
	Point3* verts = mesh.verts;

	Point3 v0;
	Point3 v1;
	Point3 v2;

	Tab<Point3> faceNormals;

	GXUInt totalVertices = (GXUInt)mesh.getNumVerts ();
	GXUInt totalFaces = (GXUInt)mesh.getNumFaces ();

	normals.SetCount ( (int)totalVertices );
	faceNormals.SetCount ( (int)totalFaces );

	for ( GXUInt i = 0; i < totalVertices; i++ ) 
		normals[ i ] = GXVertexNormal ();

	for ( GXUInt i = 0; i < totalFaces; i++, face++ ) 
	{
		v0 = verts[ face->v[ 0 ] ];
		v1 = verts[ face->v[ 1 ] ];
		v2 = verts[ face->v[ 2 ] ];

		faceNormals[ i ] = ( v1 - v0 ) ^ ( v2 - v1 );

		for ( GXUInt j = 0; j < 3; j++ ) 
			normals[ face->v[ j ] ].AddNormal ( faceNormals[ i ], face->smGroup );

		faceNormals[ i ] = Normalize( faceNormals[ i ] );
	}

	for ( GXUInt i = 0; i < totalVertices; i++ ) 
		normals[ i ].Normalize ();
}

GXVoid GXMeshNormalHolder::BuildStrips ()
{
	mesh.BuildStripsAndEdges ();
	StripTab* pStab = mesh.stab;
	GXUInt c = (GXUInt)pStab->Count ();

	for ( GXUInt i = 0; i < c; i++ )
	{
		Strip* pS = (*pStab)[ i ];

		GXChar str[ 100 ];
		sprintf_s ( str, 99, "v : %i, t : %i, n : %i", pS->v.Count (), pS->tv.Count (), pS->n.Count () );

		MessageBoxA ( 0, str, "GX Utility", MB_ICONINFORMATION );
	}
}

GXVoid GXMeshNormalHolder::GetNormal ( GXVec3 &outNormal, GXUInt index, GXDword smoothGroup ) const
{
	memcpy ( &outNormal, &( normals[ index ].GetNormal ( smoothGroup ) ), sizeof ( GXVec3 ) );
}