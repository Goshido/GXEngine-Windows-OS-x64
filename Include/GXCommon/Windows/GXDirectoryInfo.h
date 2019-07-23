// version 1.0

#ifndef GX_DIRECTORY_INFO_WINDOWS
#define GX_DIRECTORY_INFO_WINDOWS


#include <GXCommon/GXDirectoryInfo.h>


class GXDirectoryInfo final : public GXAbstractDirectoryInfo
{
    public:
        explicit GXDirectoryInfo ( GXString targetDirectory );
        ~GXDirectoryInfo () override;

    private:
        GXDirectoryInfo () = delete;
        GXDirectoryInfo ( const GXDirectoryInfo &other ) = delete;
        GXDirectoryInfo& operator = ( const GXDirectoryInfo &other ) = delete;
};


#endif // GX_DIRECTORY_INFO_WINDOWS
