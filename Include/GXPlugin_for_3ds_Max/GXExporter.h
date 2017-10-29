//version 1.0

#ifndef GX_EXPORTER
#define GX_EXPORTER


#include <GXCommon/GXTypes.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <IGame/IGame.h>
#include <GXCommon/GXRestoreWarnings.h>


class GXExporter
{
	protected:
		INode&		selection;
		GXUTF8*		fileName;

	public:
		explicit GXExporter ( INode &selection, const GXUTF8* fileName );
		virtual ~GXExporter ();

		//Returns UTF-8 string allocated in heap or nullptr. Method uses malloc operation. Caller MUST call free.
		virtual GXUTF8* GetWarnings () const;

		//Returns UTF-8 string allocated in heap or nullptr. Method uses malloc operation. Caller MUST call free.
		virtual GXUTF8* GetReport () const;

	protected:
		virtual GXBool Export () = 0;

	private:
		GXExporter ( const GXExporter &other ) = delete;
		GXExporter& operator = ( const GXExporter &other ) = delete;
};


#endif //GX_EXPORTER
