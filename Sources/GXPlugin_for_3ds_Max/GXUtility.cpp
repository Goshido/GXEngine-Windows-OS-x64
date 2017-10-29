//vesion 1.2

#include <GXPlugin_for_3ds_Max/GXUtility.h>
#include <GXPlugin_for_3ds_Max/GXMeshExporter.h>
#include <GXPlugin_for_3ds_Max/GXSkinExporter.h>
#include <GXPlugin_for_3ds_Max/GXSkeletonExporter.h>
#include <GXPlugin_for_3ds_Max/GXAnimationExporter.h>
#include <GXCommon/GXSTGStructs.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <Shobjidl.h>
#include <GXCommon/GXRestoreWarnings.h>


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

//Returns zero terminated string allocated in heap or nullptr. Allocating is done via malloc. Caller MUST call free.
GXWChar* GXCALL GetFileName ( GXUInt filterCount, const COMDLG_FILTERSPEC* filters, const GXWChar* defaultExtension )
{
	OleInitialize ( 0 );

	IFileSaveDialog* fileSaveDialog = nullptr;
	CoCreateInstance ( CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS ( &fileSaveDialog ) );

	FILEOPENDIALOGOPTIONS flags;
	fileSaveDialog->GetOptions ( &flags );
	fileSaveDialog->SetOptions ( flags | FOS_FORCEFILESYSTEM );

	fileSaveDialog->SetDefaultExtension ( defaultExtension );
	fileSaveDialog->SetFileTypes ( filterCount, filters );

	fileSaveDialog->Show ( 0 );

	IShellItem* result = nullptr;
	GXWChar* fileName = nullptr;
	fileSaveDialog->GetResult ( &result );

	if ( result )
	{
		PWSTR buffer = nullptr;
		result->GetDisplayName ( SIGDN_FILESYSPATH, &buffer );
		GXWcsclone ( &fileName, buffer );
		CoTaskMemFree ( buffer );
		result->Release ();
	}

	OleUninitialize ();

	return fileName;
}

INT_PTR CALLBACK GXUtilityDlgProc ( HWND dlgHwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			SendDlgItemMessage ( dlgHwnd, GX_UI_CHECKBOX_CURRENT_POSE, BM_SETCHECK, BST_UNCHECKED, 0 );
		}
		return TRUE;

		case WM_COMMAND:
		{
			GXUShort id = LOWORD ( wParam );

			static GXWChar buffer[ 500 ];

			switch ( id )
			{
				case GX_UI_BUTTON_BROWSE_MESH_FILE:
				{
					static const COMDLG_FILTERSPEC filters[ 1 ] = { { L"mesh", L"*.mesh" } };
					GXWChar* fileName = GetFileName ( 1, filters, L"mesh" );

					if ( fileName )
					{
						ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_MESH_FILE ) );
						edit->SetText ( fileName );
						ReleaseICustEdit ( edit );

						free ( fileName );
					}
				}
				break;

				case GX_UI_BUTTON_EXPORT_MESH:
				{
					Interface7* intrf = GetCOREInterface7 ();

					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_MESH_FILE ) );
					edit->GetText ( buffer, 499 );
					ReleaseICustEdit ( edit );

					if ( buffer[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXUTF8* fileName = nullptr;
					GXToUTF8 ( &fileName, buffer );
					GXMeshExporter exporter ( *selected, fileName, IsDlgButtonChecked ( dlgHwnd, GX_UI_CHECKBOX_CURRENT_POSE ) ? GX_TRUE : GX_FALSE );
					free ( fileName );
				}
				break;

				case GX_UI_BUTTON_BROWSE_SKIN_FILE:
				{
					static const COMDLG_FILTERSPEC filters[ 1 ] = { { L"skin", L"*.skin" } };
					GXWChar* fileName = GetFileName ( 1, filters, L"skin" );

					if ( fileName )
					{
						ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_SKIN_FILE ) );
						edit->SetText ( fileName );
						ReleaseICustEdit ( edit );

						free ( fileName );
					}
				}
				break;

				case GX_UI_BUTTON_EXPORT_SKIN:
				{
					Interface7* intrf = GetCOREInterface7 ();

					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_SKIN_FILE ) );
					edit->GetText ( buffer, 499 );
					ReleaseICustEdit ( edit );

					if ( buffer[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXUTF8* fileName = nullptr;
					GXToUTF8 ( &fileName, buffer );
					GXSkinExporter exporter ( *selected, fileName );
					free ( fileName );
				}
				break;

				case GX_UI_BUTTON_BROWSE_SKELETON_FILE:
				{
					static const COMDLG_FILTERSPEC filters[ 1 ] = { { L"skeleton", L"*.skeleton" } };
					GXWChar* fileName = GetFileName ( 1, filters, L"skeleton" );

					if ( fileName )
					{
						ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_SKELETON_FILE ) );
						edit->SetText ( fileName );
						ReleaseICustEdit ( edit );

						free ( fileName );
					}
				}
				break;

				case GX_UI_BUTTON_EXPORT_SKELETON:
				{
					Interface7* intrf = GetCOREInterface7 ();

					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_SKELETON_FILE ) );
					edit->GetText ( buffer, 499 );
					ReleaseICustEdit ( edit );

					if ( buffer[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXUTF8* fileName = nullptr;
					GXToUTF8 ( &fileName, buffer );
					GXSkeletonExporter exporter ( *selected, fileName );
					free ( fileName );
				}
				break;

				case GX_UI_BUTTON_BROWSE_ANIMATION_FILE:
				{
					static const COMDLG_FILTERSPEC filters[ 1 ] = { { L"animation", L"*.animation" } };
					GXWChar* fileName = GetFileName ( 1, filters, L"animation" );

					if ( fileName )
					{
						ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_ANIMATION_FILE ) );
						edit->SetText ( fileName );
						ReleaseICustEdit ( edit );

						free ( fileName );
					}
				}
				break;

				case GX_UI_BUTTON_EXPORT_ANIMATION:
				{
					Interface7* intrf = GetCOREInterface7 ();

					if ( intrf->GetSelNodeCount () != 1 )
					{
						MessageBoxA ( 0, "Need to select one object", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONINFORMATION );
						break;
					}

					INode* selected = intrf->GetSelNode ( 0 );

					ICustEdit* edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_ANIMATION_FILE ) );
					edit->GetText ( buffer, 499 );
					ReleaseICustEdit ( edit );

					if ( buffer[ 0 ] == 0 )
					{
						MessageBoxA ( 0, "No file name", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );
						break;
					}

					GXUTF8* fileName = nullptr;
					GXToUTF8 ( &fileName, buffer );

					edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_ANIMATION_START_FRAME ) );
					BOOL startFlag;
					GXUInt startFrame = (GXUInt)edit->GetInt ( &startFlag );
					ReleaseICustEdit ( edit );

					edit = GetICustEdit ( GetDlgItem ( dlgHwnd, GX_UI_EDITBOX_ANIMATION_LAST_FRAME ) );
					BOOL lastFlag;
					GXUInt lastFrame = (GXUInt)edit->GetInt ( &lastFlag );
					ReleaseICustEdit ( edit );

					if ( startFlag == TRUE && lastFlag == TRUE )
						GXAnimationExporter exporter ( *selected, fileName, startFrame, lastFrame );
					else
						MessageBoxA ( 0, "Start or last frame is incorrect", GX_ENGINE_EXPORTER_MESSAGE_BOX_TITLE, MB_ICONWARNING );

					free ( fileName );
				}
				break;

				default:
					//NOTHING
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