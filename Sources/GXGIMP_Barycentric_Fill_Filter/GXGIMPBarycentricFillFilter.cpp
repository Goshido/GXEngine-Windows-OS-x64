#include <GXGIMP_Barycentric_Fill_Filter/GXGimpBarycentricFillFilter.h>
#include <GXCommon/GXMath.h>


#define PLUGIN_NAME							"gx-barycentric-fill-filter"
#define MENU_NAME							"GX barycentric fill"
#define MENU_HINT							"Will be barycentric fill"
#define AUTHOR								"GXEngine"
#define COPYRIGHT							"Copyright GXEngine"
#define DATE								"30.06.2017"
#define PLUGIN_MENU_PATH					"<Image>/Filters"
#define LAYER_NAME							"GX barycentric fill"

#define INVALID_IMAGE_ID					0x7FFFFFFF
#define LAYER_OPACITY						100.0

#define PARAM_COUNT							2
#define RETURN_VALUES						1

#define MAIN_PANEL_TITLE					"GX barycentric fill"
#define MAIN_PANEL_GTK_ROLE					"GXEngine"
#define MAIN_PANEL_GTK_DIALOG_FLAGS			(GtkDialogFlags)0
#define MAIN_PANEL_HELP_ID					"GX barycentric fill"

#define APPLY_BUTTON_CAPTION				"Apply"
#define CANCEL_BUTTON_CAPTION				"Cancel"

#define MARGIN								12
#define VERTICAL_SPACING					8

#define CONTENT_ALIGNMENT_X					0.0f
#define CONTENT_ALIGNMENT_Y					0.0f
#define CONTENT_ALIGNMENT_SCALE_X			1.0f
#define CONTENT_ALIGNMENT_SCALE_Y			1.0f

#define TABLE_LAYOUT_COLUMNS				2
#define TABLE_LAYOUT_ROWS					4

#define TABLE_ALIGNMENT_PADDING_BOTTOM		0.0f
#define TABLE_ALIGNMENT_PADDING_TOP			0.0f
#define TABLE_ROW_SPACING					7

#define LABEL_ALIGNMENT_X					0.0f
#define LABEL_ALIGNMENT_Y					0.0f
#define LABEL_ALIGNMENT_SCALE_X				0.0f
#define LABEL_ALIGNMENT_SCALE_Y				1.0f
#define LABEL_ALIGNMENT_RIGHT_PADDING		77
#define LABEL_ALIGNMENT_LEFT_PADDING		0
#define LABEL_ALIGNMENT_BOTTOM_PADDING		0
#define LABEL_ALIGNMENT_TOP_PADDING			0

#define LABEL_COLOR_A_TEXT					"Color A"

#define DEFAULT_COLOR_A_RED					0
#define DEFAULT_COLOR_A_GREEN				0
#define DEFAULT_COLOR_A_BLUE				0

#define LABEL_COLOR_B_TEXT					"Color B"

#define DEFAULT_COLOR_B_RED					29478
#define DEFAULT_COLOR_B_GREEN				47421
#define DEFAULT_COLOR_B_BLUE				0

#define LABEL_COLOR_C_TEXT					"Color C"

#define DEFAULT_COLOR_C_RED					0xFFFF
#define DEFAULT_COLOR_C_GREEN				0xFFFF
#define DEFAULT_COLOR_C_BLUE				0xFFFF

#define UNUSED_PIXEL						0xFFFFFFFF

#define COLOR_C_RED							29478
#define COLOR_C_GREEN						47421
#define COLOR_C_BLUE						0

#define INPUT_CONTROL_ALIGNMENT_X			0.0f
#define INPUT_CONTROL_ALIGNMENT_Y			0.0f
#define INPUT_CONTROL_ALIGNMENT_SCALE_X		1.0f
#define INPUT_CONTROL_ALIGNMENT_SCALE_Y		0.0f

#define LABEL_SIDE_LENGTH_TEXT				"Side length (pixels)"

#define SIDE_LENGTH_STEP					1.0
#define SIDE_LENGTH_PRECISION				0
#define MINIMUM_SIDE_LENGTH					1.0
#define MAXIMUM_SIDE_LENGTH					65535.0
#define DEFAULT_SIDE_LENGTH					77.0

#define SIDE_LENGTH_ALIGNMENT_X				0.0f
#define SIDE_LENGTH_ALIGNMENT_Y				0.0f
#define SIDE_LENGTH_ALIGNMENT_SCALE_X		0.0f
#define SIDE_LENGTH_ALIGNMENT_SCALE_Y		0.0f

#define SEPARATOR_ALIGNMENT_X				0.0f
#define SEPARATOR_ALIGNMENT_Y				1.0f
#define SEPARATOR_ALIGNMENT_SCALE_X			1.0f
#define SEPARATOR_ALIGNMENT_SCALE_Y			0.0f

#define EQULATERAL_TRIANGLE_ANGLE_DEGREES	60.0f


struct GXColor
{
	guchar	r;
	guchar	g;
	guchar	b;
	guchar	a;
};


GXGIMPBarycentricFillFilter* GXGIMPBarycentricFillFilter::instance = nullptr;

GXGIMPBarycentricFillFilter& GXGIMPBarycentricFillFilter::GetInstance ()
{
	if ( !instance )
		instance = new GXGIMPBarycentricFillFilter ();

	return *instance;
}

GXGIMPBarycentricFillFilter::~GXGIMPBarycentricFillFilter ()
{
	gtk_widget_destroy ( mainPanel );

	mainPanel = nullptr;
	colorA = nullptr;
	colorB = nullptr;
	colorC = nullptr;
	sideLength = nullptr;

	imageID = INVALID_IMAGE_ID;

	instance = nullptr;
}

GXVoid GXGIMPBarycentricFillFilter::Query ()
{
	static GimpParamDef params[ PARAM_COUNT ];
	static gchar firstParameterName[] = "run-mode";
	static gchar firstParameterDescription[] = "The run mode";
	static gchar secondParameterName[] = "image";
	static gchar secondParameterDescription[] = "Input image";

	params[ 0 ].type = GIMP_PDB_INT32;
	params[ 0 ].name = firstParameterName;
	params[ 0 ].description = firstParameterDescription;

	params[ 1 ].type = GIMP_PDB_IMAGE;
	params[ 1 ].name = secondParameterName;
	params[ 1 ].description = secondParameterDescription;

	gimp_install_procedure ( PLUGIN_NAME, MENU_HINT, MENU_HINT, AUTHOR, COPYRIGHT, DATE, MENU_NAME, "RGB*", GIMP_PLUGIN, PARAM_COUNT, 0, params, nullptr );
	gimp_plugin_menu_register ( PLUGIN_NAME, PLUGIN_MENU_PATH );
}

GXVoid GXGIMPBarycentricFillFilter::Run ( const gchar* /*name*/, gint /*numParams*/, const GimpParam* params, gint* numReturnValues, GimpParam** returnValues )
{
	static GimpParam values[ RETURN_VALUES ];
	values[ 0 ].type = GIMP_PDB_STATUS;
	values[ 0 ].data.d_status = GIMP_PDB_SUCCESS;

	*numReturnValues = 1;
	*returnValues = values;

	imageID = params[ 1 ].data.d_int32;

	gtk_widget_show ( mainPanel );
	gtk_main ();
}

GXGIMPBarycentricFillFilter::GXGIMPBarycentricFillFilter ()
{
	imageID = INVALID_IMAGE_ID;
	gimp_ui_init ( PLUGIN_NAME, TRUE );

	mainPanel = gimp_dialog_new ( MAIN_PANEL_TITLE, MAIN_PANEL_GTK_ROLE, nullptr, MAIN_PANEL_GTK_DIALOG_FLAGS, &gimp_standard_help_func, MAIN_PANEL_HELP_ID, CANCEL_BUTTON_CAPTION, GTK_RESPONSE_CANCEL, APPLY_BUTTON_CAPTION, GTK_RESPONSE_APPLY, nullptr );

	gimp_window_set_transient ( GTK_WINDOW ( mainPanel ) );
	g_signal_connect ( mainPanel, "response", G_CALLBACK ( &GXGIMPBarycentricFillFilter::OnDialogResponse ), this );
	g_signal_connect ( mainPanel, "delete-event", G_CALLBACK ( &GXGIMPBarycentricFillFilter::OnCloseDialog ), this );

	GtkWidget* clientArea = gtk_dialog_get_content_area ( GTK_DIALOG ( mainPanel ) );
	GtkWidget* contentAlignment = gtk_alignment_new ( CONTENT_ALIGNMENT_X, CONTENT_ALIGNMENT_Y, CONTENT_ALIGNMENT_SCALE_X, CONTENT_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( contentAlignment );

	gtk_alignment_set_padding ( GTK_ALIGNMENT ( contentAlignment ), MARGIN, MARGIN, MARGIN, MARGIN );

	GtkWidget* contentArea = gtk_vbox_new ( FALSE, VERTICAL_SPACING );
	gtk_widget_show ( contentArea );

	gtk_container_add ( GTK_CONTAINER ( contentAlignment ), contentArea );
	gtk_box_pack_start ( GTK_BOX ( clientArea ), contentAlignment, TRUE, TRUE, MARGIN );

	GtkWidget* tableLayout = gtk_table_new ( TABLE_LAYOUT_ROWS, TABLE_LAYOUT_COLUMNS, FALSE );
	gtk_widget_show ( tableLayout );

	gtk_table_set_row_spacing ( GTK_TABLE ( tableLayout ), 0, TABLE_ROW_SPACING );
	gtk_table_set_row_spacing ( GTK_TABLE ( tableLayout ), 1, TABLE_ROW_SPACING );
	gtk_table_set_row_spacing ( GTK_TABLE ( tableLayout ), 2, TABLE_ROW_SPACING );

	gtk_box_pack_start ( GTK_BOX ( contentArea ), tableLayout, FALSE, FALSE, 0 );

	GtkAttachOptions attachX = (GtkAttachOptions)( GtkAttachOptions::GTK_FILL | GtkAttachOptions::GTK_EXPAND );

	GtkWidget* labelAlignment = gtk_alignment_new ( LABEL_ALIGNMENT_X, LABEL_ALIGNMENT_Y, LABEL_ALIGNMENT_SCALE_X, LABEL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( labelAlignment );
	gtk_alignment_set_padding ( GTK_ALIGNMENT ( labelAlignment ), LABEL_ALIGNMENT_TOP_PADDING, LABEL_ALIGNMENT_BOTTOM_PADDING, LABEL_ALIGNMENT_LEFT_PADDING, LABEL_ALIGNMENT_RIGHT_PADDING );

	GtkWidget* labelColor = gtk_label_new ( LABEL_COLOR_A_TEXT );
	gtk_widget_show ( labelColor );

	gtk_container_add ( GTK_CONTAINER ( labelAlignment ), labelColor );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), labelAlignment, 0, 1, 0, 1, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	GtkWidget* inputControlAlignment = gtk_alignment_new ( INPUT_CONTROL_ALIGNMENT_X, INPUT_CONTROL_ALIGNMENT_Y, INPUT_CONTROL_ALIGNMENT_SCALE_X, INPUT_CONTROL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( inputControlAlignment );

	colorA = gtk_color_button_new ();
	gtk_widget_show ( colorA );

	GdkColor color;
	color.red = DEFAULT_COLOR_A_RED;
	color.green = DEFAULT_COLOR_A_GREEN;
	color.blue = DEFAULT_COLOR_A_BLUE;
	color.pixel = UNUSED_PIXEL;

	gtk_color_button_set_color ( GTK_COLOR_BUTTON ( colorA ), &color );

	gtk_container_add ( GTK_CONTAINER ( inputControlAlignment ), colorA );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), inputControlAlignment, 1, 2, 0, 1, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	labelAlignment = gtk_alignment_new ( LABEL_ALIGNMENT_X, LABEL_ALIGNMENT_Y, LABEL_ALIGNMENT_SCALE_X, LABEL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( labelAlignment );
	gtk_alignment_set_padding ( GTK_ALIGNMENT ( labelAlignment ), LABEL_ALIGNMENT_TOP_PADDING, LABEL_ALIGNMENT_BOTTOM_PADDING, LABEL_ALIGNMENT_LEFT_PADDING, LABEL_ALIGNMENT_RIGHT_PADDING );

	labelColor = gtk_label_new ( LABEL_COLOR_B_TEXT );
	gtk_widget_show ( labelColor );

	gtk_container_add ( GTK_CONTAINER ( labelAlignment ), labelColor );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), labelAlignment, 0, 1, 1, 2, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	inputControlAlignment = gtk_alignment_new ( INPUT_CONTROL_ALIGNMENT_X, INPUT_CONTROL_ALIGNMENT_Y, INPUT_CONTROL_ALIGNMENT_SCALE_X, INPUT_CONTROL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( inputControlAlignment );

	colorB = gtk_color_button_new ();
	gtk_widget_show ( colorB );

	color.red = DEFAULT_COLOR_B_RED;
	color.green = DEFAULT_COLOR_B_GREEN;
	color.blue = DEFAULT_COLOR_B_BLUE;
	color.pixel = UNUSED_PIXEL;

	gtk_color_button_set_color ( GTK_COLOR_BUTTON ( colorB ), &color );

	gtk_container_add ( GTK_CONTAINER ( inputControlAlignment ), colorB );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), inputControlAlignment, 1, 2, 1, 2, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	labelAlignment = gtk_alignment_new ( LABEL_ALIGNMENT_X, LABEL_ALIGNMENT_Y, LABEL_ALIGNMENT_SCALE_X, LABEL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( labelAlignment );
	gtk_alignment_set_padding ( GTK_ALIGNMENT ( labelAlignment ), LABEL_ALIGNMENT_TOP_PADDING, LABEL_ALIGNMENT_BOTTOM_PADDING, LABEL_ALIGNMENT_LEFT_PADDING, LABEL_ALIGNMENT_RIGHT_PADDING );

	labelColor = gtk_label_new ( LABEL_COLOR_C_TEXT );
	gtk_widget_show ( labelColor );

	gtk_container_add ( GTK_CONTAINER ( labelAlignment ), labelColor );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), labelAlignment, 0, 1, 2, 3, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	inputControlAlignment = gtk_alignment_new ( INPUT_CONTROL_ALIGNMENT_X, INPUT_CONTROL_ALIGNMENT_Y, INPUT_CONTROL_ALIGNMENT_SCALE_X, INPUT_CONTROL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( inputControlAlignment );

	colorC = gtk_color_button_new ();
	gtk_widget_show ( colorC );

	color.red = DEFAULT_COLOR_C_RED;
	color.green = DEFAULT_COLOR_C_GREEN;
	color.blue = DEFAULT_COLOR_C_BLUE;
	color.pixel = UNUSED_PIXEL;

	gtk_color_button_set_color ( GTK_COLOR_BUTTON ( colorC ), &color );

	gtk_container_add ( GTK_CONTAINER ( inputControlAlignment ), colorC );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), inputControlAlignment, 1, 2, 2, 3, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	labelAlignment = gtk_alignment_new ( LABEL_ALIGNMENT_X, LABEL_ALIGNMENT_Y, LABEL_ALIGNMENT_SCALE_X, LABEL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( labelAlignment );
	gtk_alignment_set_padding ( GTK_ALIGNMENT ( labelAlignment ), LABEL_ALIGNMENT_TOP_PADDING, LABEL_ALIGNMENT_BOTTOM_PADDING, LABEL_ALIGNMENT_LEFT_PADDING, LABEL_ALIGNMENT_RIGHT_PADDING );

	GtkWidget* labelSideLength = gtk_label_new ( LABEL_SIDE_LENGTH_TEXT );
	gtk_widget_show ( labelSideLength );

	gtk_container_add ( GTK_CONTAINER ( labelAlignment ), labelSideLength );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), labelAlignment, 0, 1, 3, 4, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	inputControlAlignment = gtk_alignment_new ( INPUT_CONTROL_ALIGNMENT_X, INPUT_CONTROL_ALIGNMENT_Y, INPUT_CONTROL_ALIGNMENT_SCALE_X, INPUT_CONTROL_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( inputControlAlignment );

	GtkObject* adjustmentObject = gtk_adjustment_new ( DEFAULT_SIDE_LENGTH, MINIMUM_SIDE_LENGTH, MAXIMUM_SIDE_LENGTH, SIDE_LENGTH_STEP, SIDE_LENGTH_STEP, 0.0 );
	sideLength = gtk_spin_button_new ( GTK_ADJUSTMENT ( adjustmentObject ), SIDE_LENGTH_STEP, SIDE_LENGTH_PRECISION );
	gtk_widget_show ( sideLength );

	GtkWidget* sideLengthAlignment = gtk_alignment_new ( SIDE_LENGTH_ALIGNMENT_X, SIDE_LENGTH_ALIGNMENT_Y, SIDE_LENGTH_ALIGNMENT_SCALE_X, SIDE_LENGTH_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( sideLengthAlignment );

	gtk_spin_button_set_numeric ( GTK_SPIN_BUTTON ( sideLength ), TRUE );
	gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( sideLength ), MINIMUM_SIDE_LENGTH, MAXIMUM_SIDE_LENGTH );

	gtk_container_add ( GTK_CONTAINER ( inputControlAlignment ), sideLength );
	gtk_table_attach ( GTK_TABLE ( tableLayout ), inputControlAlignment, 1, 2, 3, 4, attachX, GtkAttachOptions::GTK_SHRINK, 0, 0 );

	GtkWidget* separatorAlignment = gtk_alignment_new ( SEPARATOR_ALIGNMENT_X, SEPARATOR_ALIGNMENT_Y, SEPARATOR_ALIGNMENT_SCALE_X, SEPARATOR_ALIGNMENT_SCALE_Y );
	gtk_widget_show ( separatorAlignment );

	GtkWidget* separator = gtk_hseparator_new ();
	gtk_widget_show ( separator );

	gtk_container_add ( GTK_CONTAINER ( separatorAlignment ), separator );
	gtk_box_pack_start ( GTK_BOX ( contentArea ), separatorAlignment, TRUE, TRUE, 0 );
}

GXVoid GXGIMPBarycentricFillFilter::ApplyFilter ()
{
	if ( imageID == INVALID_IMAGE_ID ) return;

	gint side = (gint)gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( sideLength ) );
	gint height = (gint)( (GXFloat)side * sinf ( GXDegToRad ( EQULATERAL_TRIANGLE_ANGLE_DEGREES ) ) );

	gint32 layerID = gimp_layer_new ( imageID, LAYER_NAME, side, height, GimpImageType::GIMP_RGBA_IMAGE, LAYER_OPACITY, GimpLayerModeEffects::GIMP_NORMAL_MODE );
	gimp_image_insert_layer ( imageID, layerID, 0, -1 );
	GimpDrawable* layer = gimp_drawable_get ( layerID );

	GimpPixelRgn pixelRegion;
	gimp_pixel_rgn_init ( &pixelRegion, layer, 0, 0, side, height, TRUE, TRUE );

	GXColor* colorBuffer = (GXColor*)malloc ( side * height * sizeof ( GXColor ) );
	GXUPointer offset = 0;

	for ( gint x = 0; x < side; x++ )
	{
		for ( gint y = 0; y < height; y++ )
		{
			GXColor& c = colorBuffer[ offset ];
			c.r = 115;
			c.g = 185;
			c.b = 0;
			c.a = 255;

			offset++;
		}
	}

	gimp_pixel_rgn_set_rect ( &pixelRegion, (const guchar*)colorBuffer, 0, 0, side, height );

	gimp_drawable_flush ( layer );
	gimp_drawable_merge_shadow ( layerID, TRUE );
	gimp_drawable_update ( layerID, 0, 0, side, height );
	gimp_displays_flush ();
	gimp_drawable_detach ( layer );

	free ( colorBuffer );
}

void GXGIMPBarycentricFillFilter::OnDialogResponse ( GtkWidget* /*widget*/, gint responseID, gpointer data )
{
	GXGIMPBarycentricFillFilter* barycentricFillFilter = (GXGIMPBarycentricFillFilter*)data;

	switch ( responseID )
	{
		case GTK_RESPONSE_APPLY:
			barycentricFillFilter->ApplyFilter ();
			gtk_widget_hide ( barycentricFillFilter->mainPanel );
			gtk_main_quit ();
		break;

		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide ( barycentricFillFilter->mainPanel );
			gtk_main_quit ();
		break;

		default:
			//NOTHING
		break;
	}
}

gboolean GXGIMPBarycentricFillFilter::OnCloseDialog ( GtkWidget* /*widget*/, GdkEvent* /*event*/, gpointer data )
{
	GXGIMPBarycentricFillFilter* barycentricFillFilter = (GXGIMPBarycentricFillFilter*)data;
	gtk_widget_hide ( barycentricFillFilter->mainPanel );
	gtk_main_quit ();

	return TRUE;
}
