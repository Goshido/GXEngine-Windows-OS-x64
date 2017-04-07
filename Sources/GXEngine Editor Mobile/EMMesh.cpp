#include <GXEngine_Editor_Mobile/EMMesh.h>
#include <GXCommon/GXStrings.h>


EMMesh::EMMesh ( const GXWChar* staticMeshFileName )
{
	GXWcsclone ( &meshFileName, staticMeshFileName );
	InitGraphicResources ();
}

EMMesh::~EMMesh ()
{
	free ( meshFileName );
	GXMeshGeometry::RemoveMeshGeometry ( meshGeometry );
}

GXVoid EMMesh::Render ()
{
	meshGeometry.Render ();
}

GXVoid EMMesh::InitGraphicResources ()
{
	meshGeometry = GXMeshGeometry::LoadFromStm ( meshFileName );
	TransformUpdated ();
}

GXVoid EMMesh::TransformUpdated ()
{
	GXSetAABBTransformed ( boundsWorld, meshGeometry.GetBoundsLocal (), mod_mat );
}
