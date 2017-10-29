//version 1.2

#ifndef GX_UTILITY
#define GX_UTILITY


#include "GXDLL.h"
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <utilapi.h>
#include <GXCommon/GXRestoreWarnings.h>


#define GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE		"GXEngine exporter"

class GXUtility : public UtilityObj
{
	private:
		HWND		ui;

	public:
		GXUtility ();
		~GXUtility () override;

		void BeginEditParams ( Interface* ip,IUtil* iu ) override;
		void EndEditParams ( Interface* ip, IUtil* iu) override;

		void SelectionSetChanged ( Interface* ip, IUtil* iu ) override;
		void DeleteThis () override;
		void SetStartupParam ( MSTR param ) override;
};

//----------------------------------------------------------------------------------------

class GXUtilityDesc : public ClassDesc2
{
	public:
		int IsPublic () override;
		void* Create ( BOOL loading ) override;
		const MCHAR* ClassName () override;
		SClass_ID SuperClassID () override;
		Class_ID ClassID () override;
		const MCHAR* Category () override;
};

//----------------------------------------------------------------------------------------

ClassDesc* GXCALL GXGetUtilityDesc ();


#endif //GX_UTILITY
