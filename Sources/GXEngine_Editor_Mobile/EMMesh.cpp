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
	GXMeshGeometry::RemoveMeshGeometry ( meshGeometry );
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
	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, meshFileName );

	if ( GXWcscmp ( extension, L"stm") == 0 || GXWcscmp ( extension, L"STM" ) == 0 )
		meshGeometry = GXMeshGeometry::LoadFromStm ( meshFileName );
	else if ( GXWcscmp ( extension, L"skm" ) == 0 || GXWcscmp ( extension, L"SKM" ) == 0 )
		meshGeometry = GXMeshGeometry::LoadFromSkm ( meshFileName );
	else
		meshGeometry = GXMeshGeometry::LoadFromObj ( meshFileName );

	GXSafeFree ( extension );

	TransformUpdated ();
}

GXVoid EMMesh::TransformUpdated ()
{
	GXSetAABBTransformed ( boundsWorld, meshGeometry.GetBoundsLocal (), currentFrameModelMatrix );
}
