PLUGIN_NAME=gx-barycentric-fill.exe
RELEASE_DIRECTORY=../../Release/x64/

CPPFLAGS=-Wall -pthread -mms-bitfields -pthread -mms-bitfields -mwindows

LIBRARY_DIRECTORIES=\
    -L$(MINGW_X64_ROOT_DIR)/lib

LIBRARIES=\
    -lgimpui-2.0 \
    -lgimpwidgets-2.0 \
    -lgimpmodule-2.0 \
    -lgimp-2.0 \
    -lbabl-0.1 \
    -lgtk-win32-2.0 \
    -lgdk-win32-2.0 \
    -lgdi32 \
    -limm32 \
    -limm32 \
    -lshell32 \
    -lole32 \
    -Wl,-luuid \
    -lpangowin32-1.0 \
    -lpangocairo-1.0 \
    -lpango-1.0 \
    -latk-1.0 \
    -lcairo \
    -lgdk_pixbuf-2.0 \
    -lgio-2.0 \
    -lgobject-2.0 \
    -lglib-2.0 \
    -lintl

SOURCES=\
    ../../Sources/GXGIMP_Barycentric_Fill_Filter/GXGIMPBarycentricFillFilter.cpp \
    ../../Sources/GXGIMP_Barycentric_Fill_Filter/main.cpp

INCLUDE_DIRECTORIES=\
    -I$(GX_ENGINE_SDK_DIR) \
    -I$(MINGW_X64_ROOT_DIR)/x86_64-w64-mingw32/include \
    -I$(GIMP_SDK_DIR) \
    -I$(CAIRO_SDK_DIR) \
    -I$(BABL_SDK_DIR) \
    -I$(GEGL_SDK_DIR) \
    -I$(GDK_PIXBUF_SDK_DIR) \
    -I$(GLIB_SDK_DIR) \
    -I$(GLIB_SDK_CONFIG_HEADER_DIR) \
    -I$(GTK_SDK_DIR) \
    -I$(JSON_GLIB_SDK_DIR) \
    -I$(GIO_WIN_32_SDK_DIR) \
    -I$(GTK_SDK_CONFIG_HEADER_DIR) \
    -I$(PANGO_SDK_DIR) \
    -I$(ATK_SDK_DIR) \
    -I$(PIXMAN_SDK_DIR) \
    -I$(FREETYPE_SDK_DIR) \
    -I$(LIBPNG_SDK_DIR) \
    -I$(HARBUZZ_SDK_DIR)

all: compile-plugin deploy-plugin complete

compile-plugin:
	g++ $(CPPFLAGS) -o $(RELEASE_DIRECTORY)$(PLUGIN_NAME) $(INCLUDE_DIRECTORIES) $(SOURCES) $(LIBRARY_DIRECTORIES) $(LIBRARIES) 

deploy-plugin:
	cp -f "$(RELEASE_DIRECTORY)$(PLUGIN_NAME)" "$(GIMP_PLUGIN_DIRECTORY)/$(PLUGIN_NAME)"

complete:
	printf "\nГотово\n"
