#include <GXEngine_Editor_Mobile/EMMesh.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


EMMesh::EMMesh ( const GXWChar* meshFileName ):
	skinFileName ( nullptr )
{
	GXWcsclone ( &this->meshFileName, meshFileName );
	InitGraphicResources ();
}

EMMesh::EMMesh ( const GXWChar* meshFileName, const GXWChar* skinFileName )
{
	GXWcsclone ( &this->skinFileName, skinFileName );
	GXWcsclone ( &this->meshFileName, meshFileName );
	InitGraphicResources ();
}

EMMesh::~EMMesh ()
{
	free ( meshFileName );
	GXSafeFree ( skinFileName );
}

GXVoid EMMesh::Render ()
{
	meshGeometry.Render ();
}

GXVoid EMMesh::UpdatePose ( GXSkeleton &skeleton )
{
	meshGeometry.UpdatePose ( skeleton );
}

GXVoid EMMesh::InitGraphicResources ()
{
	meshGeometry.LoadMesh ( meshFileName );

	if ( skinFileName )
		meshGeometry.LoadSkin ( skinFileName );

	TransformUpdated ();
}

GXVoid EMMesh::TransformUpdated ()
{
	meshGeometry.GetBoundsLocal ().Transform ( boundsWorld, currentFrameModelMatrix );
}
