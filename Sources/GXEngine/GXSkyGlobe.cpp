//version 1.3

#include <GXEngine/GXSkyGlobe.h>


GXSkyGlobe::GXSkyGlobe ()
{
	mesh = 0;
}

GXSkyGlobe::~GXSkyGlobe ()
{
	if ( mesh ) delete mesh;
}

GXVoid GXSkyGlobe::Draw ()
{
	if ( !mesh ) return;

	GXVec3 cam_pos;
	gx_ActiveCamera->GetLocation ( cam_pos );
	mesh->SetLocation ( cam_pos.v );
	mesh->Draw ();
	glClear ( GL_DEPTH_BUFFER_BIT );
}