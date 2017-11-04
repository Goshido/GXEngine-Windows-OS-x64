#include <GXEngine_Editor_Mobile/EMMesh.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


EMMesh::EMMesh ( const GXWChar* meshFileName )
{
	GXWcsclone ( &this->meshFileName, meshFileName );
	skinFileName = nullptr;

	InitGraphicResources ();
}

EMMesh::EMMesh ( const GXWChar* meshFileName, const GXWChar* skinFileName )
{
	GXWcsclone ( &this->meshFileName, meshFileName );
	GXWcsclone ( &this->skinFileName, skinFileName );

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
