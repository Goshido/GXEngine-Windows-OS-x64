#include <GXGIMP_Barycentric_Fill_Filter/GXGIMPBarycentricFillFilter.h>
#include <libgimp/gimp.h>


static GXVoid GXGIMPPluginInit ()
{
	GXGIMPBarycentricFillFilter::GetInstance ();
}

static GXVoid GXGIMPPluginQuit ()
{
	delete &( GXGIMPBarycentricFillFilter::GetInstance () );
}

static GXVoid GXGIMPPluginQuery ()
{
	GXGIMPBarycentricFillFilter::GetInstance ().Query ();
}

static GXVoid GXGIMPPluginRun ( const gchar* name, gint numParams, const GimpParam* params, gint* numReturnValues, GimpParam** returnValues )
{
	GXGIMPBarycentricFillFilter::GetInstance ().Run ( name, numParams, params, numReturnValues, returnValues );
}

const GimpPlugInInfo PLUG_IN_INFO = { &GXGIMPPluginInit, &GXGIMPPluginQuit, &GXGIMPPluginQuery, &GXGIMPPluginRun };

MAIN ()
