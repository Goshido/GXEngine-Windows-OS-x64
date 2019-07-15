#include <GXEngine_Editor_Mobile/EMMesh.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>


EMMesh::EMMesh ( const GXWChar* meshFileName ):
    _meshFileName ( meshFileName )
{
    InitGraphicResources ();
}

EMMesh::EMMesh ( const GXWChar* meshFileName, const GXWChar* skinFileName ):
    _meshFileName ( meshFileName ),
    _skinFileName ( skinFileName )
{
    InitGraphicResources ();
}

EMMesh::~EMMesh ()
{
    // NOTHING
}

GXVoid EMMesh::Render ()
{
    _meshGeometry.Render ();
}

GXVoid EMMesh::UpdatePose ( GXSkeleton &skeleton )
{
    _meshGeometry.UpdatePose ( skeleton );
}

GXVoid EMMesh::InitGraphicResources ()
{
    _meshGeometry.LoadMesh ( _meshFileName );

    if ( !_skinFileName.IsNull () )
        _meshGeometry.LoadSkin ( _skinFileName );

    TransformUpdated ();
}

GXVoid EMMesh::TransformUpdated ()
{
    _meshGeometry.GetBoundsLocal ().Transform ( _boundsWorld, _currentFrameModelMatrix );
}
