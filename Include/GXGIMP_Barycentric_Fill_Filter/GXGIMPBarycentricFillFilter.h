#ifndef GX_GIMP_BARYCENTRIC_FILL_FILTER
#define GX_GIMP_BARYCENTRIC_FILL_FILTER


#include <GXCommon/GXTypes.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>


class GXGIMPBarycentricFillFilter
{
	private:
		GtkWidget*								mainPanel;
		GtkWidget*								colorA;
		GtkWidget*								colorB;
		GtkWidget*								colorC;
		GtkWidget*								sideLength;

		gint32									imageID;

		static GXGIMPBarycentricFillFilter*		instance;

	public:
		static GXGIMPBarycentricFillFilter& GetInstance ();
		~GXGIMPBarycentricFillFilter ();

		GXVoid Query ();
		GXVoid Run ( const gchar* name, gint numParams, const GimpParam* params, gint* numReturnValues, GimpParam** returnValues );

	private:
		GXGIMPBarycentricFillFilter ();

		GXVoid ApplyFilter ();

		static void OnDialogResponse ( GtkWidget* widget, gint responseID, gpointer data );
		static gboolean OnCloseDialog ( GtkWidget* widget, GdkEvent* event, gpointer data );
};


#endif //GX_GIMP_BARYCENTRIC_FILL_FILTER
