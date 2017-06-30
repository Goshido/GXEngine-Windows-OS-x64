#include <GXCommon/GXTypes.h>
#include <libgimp/gimp.h>


#define PLUGIN_NAME				"gx-barycentric-fill-filter"
#define MENU_NAME				"GX barycentric fill"
#define MENU_HINT				"Will be barycentric fill"
#define AUTHOR					"GXEngine"
#define COPYRIGHT				"Copyright GXEngine"
#define DATE					"30.06.2017"

#define PLUGIN_MENU_PATH		"<Image>/Filters"

#define PARAM_COUNT				3
#define RETURN_VALUES			1


static GXVoid GXGimpPluginQuery ()
{
	static GimpParamDef params[ PARAM_COUNT ];
	static gchar firstParameterName[] = "run-mode";
	static gchar firstParameterDescription[] = "The run mode";
	static gchar secondParameterName[] = "image";
	static gchar secondParameterDescription[] = "Input image";
	static gchar thirdParameterName[] = "drawable";
	static gchar thirdParameterDescription[] = "Input drawable";

	params[ 0 ].type = GIMP_PDB_INT32;
	params[ 0 ].name = firstParameterName;
	params[ 0 ].description = firstParameterDescription;

	params[ 1 ].type = GIMP_PDB_IMAGE;
	params[ 1 ].name = secondParameterName;
	params[ 1 ].description = secondParameterDescription;

	params[ 2 ].type = GIMP_PDB_DRAWABLE;
	params[ 2 ].name = thirdParameterName;
	params[ 2 ].description = thirdParameterDescription;

	gimp_install_procedure ( PLUGIN_NAME, MENU_HINT, MENU_HINT, AUTHOR, COPYRIGHT, DATE, MENU_NAME, "RGB*, GRAY*", GIMP_PLUGIN, PARAM_COUNT, 0, params, nullptr );
	gimp_plugin_menu_register ( PLUGIN_NAME, PLUGIN_MENU_PATH );
}

static GXVoid GXGimpPluginRun ( const gchar* /*name*/, gint /*numParams*/, const GimpParam* /*params*/, gint* numReturnValues, GimpParam** returnValues )
{
	static GimpParam values[ RETURN_VALUES ];
	values[ 0 ].type = GIMP_PDB_STATUS;
	values[ 0 ].data.d_status = GIMP_PDB_SUCCESS;

	*numReturnValues = 1;
	*returnValues = values;

	g_message ( "Hello, boobs!\n" );
}

const GimpPlugInInfo PLUG_IN_INFO = { nullptr, nullptr, &GXGimpPluginQuery, &GXGimpPluginRun };

MAIN ()
