//version 1.2

#include <GXEngine/GXCameraman.h>
#include <GXCommon/GXLogger.h>


GXCameraman::GXCameraman ( GXCamera* camera )
{
	this->camera = camera;
}

GXCameraman::~GXCameraman ()
{
	/*PURE VIRTUAL*/
};

GXVoid GXCameraman::SetCamera ( GXCamera* camera )
{
	if ( !camera )
	{
		GXLogW ( L"TSCameraman::SetCamera::Error - ѕопытка добавлени€ по нулевому указателю!\n" );
		return;
	}

	this->camera = camera;
}