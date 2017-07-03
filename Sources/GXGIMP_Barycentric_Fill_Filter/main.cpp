#include <GXGIMP_Barycentric_Fill_Filter/GXGIMPBarycentricFillFilter.h>
#include <libgimp/gimp.h>


static void GXGIMPPluginInit ()
{
	GXGIMPBarycentricFillFilter::GetInstance ();
}

static void GXGIMPPluginQuit ()
{
	delete &( GXGIMPBarycentricFillFilter::GetInstance () );
}

static void GXGIMPPluginQuery ()
{
	GXGIMPBarycentricFillFilter::GetInstance ().Query ();
}

static void GXGIMPPluginRun ( const gchar* name, gint numParams, const GimpParam* params, gint* numReturnValues, GimpParam** returnValues )
{
	GXGIMPBarycentricFillFilter::GetInstance ().Run ( name, numParams, params, numReturnValues, returnValues );
}

const GimpPlugInInfo PLUG_IN_INFO = { &GXGIMPPluginInit, &GXGIMPPluginQuit, &GXGIMPPluginQuery, &GXGIMPPluginRun };

MAIN ()
