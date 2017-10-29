//version 1.0

#include <GXPlugin_for_3ds_Max/GXExporter.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GXExporter::GXExporter ( INode& selection, const GXUTF8* fileName ):
selection(selection)
{
	GXUTF8clone ( &this->fileName, fileName );
}

GXExporter::~GXExporter ()
{
	GXSafeFree ( fileName );
}

GXUTF8* GXExporter::GetWarnings () const
{
	return nullptr;
}

GXUTF8* GXExporter::GetReport () const
{
	return nullptr;
}
