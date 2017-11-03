#include <GXEngine_Editor_Mobile/EMMesh.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


EMMesh::EMMesh ( const GXWChar* meshFileName )
{
	GXWcsclone ( &this->meshFileName, meshFileName );
	InitGraphicResources ();
}

EMMesh::~EMMesh ()
{
	free ( meshFileName );
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
	TransformUpdated ();
}

GXVoid EMMesh::TransformUpdated ()
{
	meshGeometry.GetBoundsLocal ().Transform ( boundsWorld, currentFrameModelMatrix );
}
