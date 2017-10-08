//vesion 1.1

#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXPlugin_for_3ds_Max/GXSkeletalMeshExporter.h>
#include <GXPlugin_for_3ds_Max/GXAnimationExporter.h>
#include <GXCommon/GXSTGStructs.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXStrings.h>


INT_PTR CALLBACK GXUtilityDlgProc ( HWND dlgHwnd, UINT msg, WPARAM wParam, LPARAM lParam );

static GXUtility		gx_Utility;
Interface*				gx_UtilityInterface = nullptr;

Class_ID				gx_UtilityClassID ( 0x5ba96f0e, 0x44345f2d );
static GXWChar			gx_UtilityClassName[] = L"GXEngine exporter";
static GXUtilityDesc	gx_UtilityDesc;


GXVoid GXConvertChar2Wchar ( GXWChar** outStr, const GXChar* inStr )
{
	GXUInt chars = (GXUInt)strlen ( inStr ) + 1;
	GXUInt size = sizeof ( GXWChar ) * chars;
	*outStr = (GXWChar*)malloc ( size );

	MultiByteToWideChar ( (UINT)CP_ACP, (DWORD)MB_PRECOMPOSED, inStr, (int)( chars * sizeof ( GXChar ) ), *outStr, (int)chars );
}

//--------------------------------------------------------------------------------------------

GXUtility::GXUtility ()
{
	//NOTHING
}

GXUtility::~GXUtility ()
{
	//NOTHING
}

void GXUtility::BeginEditParams ( Interface* ip, IUtil* /*iu*/ )
{
	gx_UtilityInterface = ip;
	ui = gx_UtilityInterface->AddRollupPage ( gx_Hinstance, MAKEINTRESOURCE ( GX_IDD_DIALOG ), &GXUtilityDlgProc, gx_UtilityClassName );
}

void GXUtility::EndEditParams ( Interface* /*ip*/, IUtil* /*iu*/ )
{
	gx_UtilityInterface->DeleteRollupPage ( ui );
}

void GXUtility::SelectionSetChanged ( Interface* /*ip*/, IUtil* /*iu*/ )
{
	//NOTHING
}

void GXUtility::DeleteThis ()
{
	//NOTHING
}

void GXUtility::SetStartupParam ( MSTR param )
{
	//NOTHING
}

//-------------------------------------------------------------------------------------

GXBool GXCALL GXUtilityExportNativeStaticMesh ( const GXWChar* fileName, INode* node, GXBool elements, GXBool isExportUVs, GXBool isExportNormals, GXBool isExportTangentBitangentPairs );

INT_PTR CALLBACK GXUtilityDlgProc ( HWND dlgHwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			SendDlgItemMessage ( dlgHwnd, GX_UI_UTL_CHK_EXPT_ELEMENTS, BM_SETCHECK, BST_UNCHECKED, 0 );
			SendDlgItemMessage ( dlgHwnd, GX_UI_UTL_CHK_EXPT_UVS, BM_SETCHECK, BST_CHECKED, 0 );
			SendDlgItemMessage ( dlgHwnd, GX_UI_UTL_CHK_EXPT_NORMALS, BM_SETCHECK, BST_CHECKED, 0 );
			SendDlgItemMessage ( dlgHwnd, GX_UI_UTL_CHK_EXPT_TBPAIRS, BM_SETCHECK, BST_CHECKED, 0 );
		}
		return TRUE;

		case WM_COMMAND:
		{
			GXUShort id = LOWORD( wParam );
			switch ( id )
			{
				case GX_UI_UTL_BTN_BRWS_SCENE:
					MessageBoxA ( 0, "Browse", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
				break;

				case GX_UI_UTL_BTN_EXPT_STM:
				{
					Interface7* intrf = GetCOREInterface7 ();
					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_UTL_EDT_STM_FILE ) );
					GXWChar buf[ 500 ];
					edit->GetText ( buf, 499 );
					ReleaseICustEdit ( edit );

					if ( buf[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXBool elements = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_ELEMENTS ) ? GX_TRUE : GX_FALSE;
					GXBool uvs = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_UVS ) ? GX_TRUE : GX_FALSE;
					GXBool normals = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_NORMALS ) ? GX_TRUE : GX_FALSE;
					GXBool tbPairs = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_TBPAIRS ) ? GX_TRUE : GX_FALSE;

					if ( GXUtilityExportNativeStaticMesh ( buf, selected, elements, uvs, normals, tbPairs ) )
						MessageBoxA ( 0, "Saving completed", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
					else
						MessageBoxA ( 0, "Saving failed", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
				}
				break;

				case GX_UI_UTL_BTN_EXPT_SKM:
				{
					Interface7* intrf = GetCOREInterface7 ();
					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_UTL_EDT_SKM_FILE ) );
					GXWChar buf[ 500 ];
					edit->GetText ( buf, 499 );
					ReleaseICustEdit ( edit );

					if ( buf[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXUTF8* fileName = nullptr;
					GXToUTF8 ( &fileName, buf );
					GXSkeletalMeshExporter exp ( *selected, fileName, nullptr );
					free ( fileName );
				}
				break;

				case GX_UI_UTL_BTN_EXPT_ANI:
				{
					Interface7* intrf = GetCOREInterface7 ();
					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_UTL_EDT_ANI_FILE ) );
					GXWChar buf[ 500 ];
					edit->GetText ( buf, 499 );
					ReleaseICustEdit ( edit );

					if ( buf[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXUTF8* fileName = nullptr;
					GXToUTF8 ( &fileName, buf );

					edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_UTL_EDT_ANI_START ) );
					BOOL startFlag;
					GXUInt startFrame = (GXUInt)edit->GetInt ( &startFlag );
					ReleaseICustEdit ( edit );

					edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_UTL_EDT_ANI_LAST ) );
					BOOL lastFlag;
					GXUInt lastFrame = (GXUInt)edit->GetInt ( &lastFlag );
					ReleaseICustEdit ( edit );

					if ( startFlag == TRUE && lastFlag == TRUE )
						GXAnimationExporter exp ( *selected, fileName, startFrame, lastFrame );
					else
						MessageBoxA ( 0, "Start or last frame is incorrect", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );

					free ( fileName );
				}
				break;

				case GX_UI_UTL_CHK_EXPT_ELEMENTS:
				case GX_UI_UTL_CHK_EXPT_UVS:
				case GX_UI_UTL_CHK_EXPT_NORMALS:
				case GX_UI_UTL_CHK_EXPT_TBPAIRS:
				{
					GXBool elements = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_ELEMENTS ) ? GX_TRUE : GX_FALSE;
					GXBool uvs = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_UVS ) ? GX_TRUE : GX_FALSE;
					GXBool normals = IsDlgButtonChecked ( dlgHwnd, GX_UI_UTL_CHK_EXPT_NORMALS ) ? GX_TRUE : GX_FALSE;

					HWND normalsControl = GetDlgItem ( dlgHwnd, GX_UI_UTL_CHK_EXPT_NORMALS );
					HWND tbPairsControl = GetDlgItem ( dlgHwnd, GX_UI_UTL_CHK_EXPT_TBPAIRS );

					if ( elements )
					{
						SendMessage ( normalsControl, BM_SETCHECK, BST_UNCHECKED, 0 );
						EnableWindow ( normalsControl, FALSE );

						SendMessage ( tbPairsControl, BM_SETCHECK, BST_UNCHECKED, 0 );
						EnableWindow ( tbPairsControl, FALSE );

						return TRUE;
					}
					else if ( !uvs )
					{
						SendMessage ( normalsControl, BM_SETCHECK, BST_UNCHECKED, 0 );
						EnableWindow ( normalsControl, FALSE );

						SendMessage ( tbPairsControl, BM_SETCHECK, BST_UNCHECKED, 0 );
						EnableWindow ( tbPairsControl, FALSE );

						return TRUE;
					}

					EnableWindow ( normalsControl, TRUE );
					EnableWindow ( tbPairsControl, TRUE );

					if ( !normals )
					{
						SendMessage ( tbPairsControl, BM_SETCHECK, BST_UNCHECKED, 0 );
						EnableWindow ( tbPairsControl, FALSE );
					}
				}
				break;
			}
		}
		return TRUE;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			gx_UtilityInterface->RollupMouseMessage ( dlgHwnd, msg, wParam, lParam );
		return TRUE;
		
		default:
			//NOTHING
		return FALSE;
	}
}

//-------------------------------------------------------------------------------------

GXBool GXCALL GXUtilityExportNativeStaticMesh ( const GXWChar* /*fileName*/, INode* /*node*/, GXBool /*elements*/, GXBool /*isExportUVs*/, GXBool /*isExportNormals*/, GXBool /*isExportTangentBitangentPairs*/ )
{
	MessageBoxA ( 0, "Need to implement", "Warning", MB_ICONWARNING | MB_OK );

	return GX_TRUE;
}

//-------------------------------------------------------------------------------------

int GXUtilityDesc::IsPublic ()
{
	return 1;
}

void* GXUtilityDesc::Create ( BOOL /*loading*/ )
{
	return &gx_Utility;
}

const MCHAR* GXUtilityDesc::ClassName ()
{
	return gx_UtilityClassName;
}

SClass_ID GXUtilityDesc::SuperClassID ()
{
	return UTILITY_CLASS_ID;
}

Class_ID GXUtilityDesc::ClassID ()
{
	return gx_UtilityClassID;
}


const MCHAR* GXUtilityDesc::Category ()
{
	return 0;
}


ClassDesc* GXCALL GXGetUtilityDesc ()
{
	return &gx_UtilityDesc;
}