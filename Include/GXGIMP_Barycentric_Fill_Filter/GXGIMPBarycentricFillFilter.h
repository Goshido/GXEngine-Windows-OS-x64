#ifndef GX_GIMP_BARYCENTRIC_FILL_FILTER
#define GX_GIMP_BARYCENTRIC_FILL_FILTER


#include <GXCommon/GXTypes.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>


class GXGIMPBarycentricFillFilter
{
	private:
		static GXGIMPBarycentricFillFilter*		instance;

	public:
		static GXGIMPBarycentricFillFilter& GetInstance ();
		~GXGIMPBarycentricFillFilter ();

		GXVoid Query ();
		GXVoid Run ( const gchar* name, gint numParams, const GimpParam* params, gint* numReturnValues, GimpParam** returnValues );

	private:
		GXGIMPBarycentricFillFilter ();

		GXVoid ShowGUI ();
		GXVoid ApplyFilter ();
};


#endif //GX_GIMP_BARYCENTRIC_FILL_FILTER
