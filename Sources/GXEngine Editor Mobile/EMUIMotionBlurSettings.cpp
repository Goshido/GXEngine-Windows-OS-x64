#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXRenderer.h>


#define LABEL_COLOR_R					255
#define LABEL_COLOR_G					255
#define LABEL_COLOR_B					255
#define LABEL_COLOR_A					255

#define MAX_BUFER_SYMBOLS				16

#define DEFAULT_MAIN_PANEL_WIDTH		8.0f
#define DEFAULT_MAIN_PANEL_HEIGHT		10.0f

#define START_MAIN_PANEL_LEFT_X_OFFSET	1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET	1.5f


EMUIMotionBlurSettings::EMUIMotionBlurSettings () :
EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( this );
	caption = new EMUIStaticText ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );
	totalSamplesLabel = new EMUIStaticText ( mainPanel );
	totalSamples = new EMUIEditBox ( mainPanel );
	depthLimitLabel = new EMUIStaticText ( mainPanel );
	depthLimit = new EMUIEditBox ( mainPanel );
	exposureLabel = new EMUIStaticText ( mainPanel );
	exposure = new EMUIEditBox ( mainPanel );
	bottomSeparator = new EMUISeparator ( mainPanel );
	cancel = new EMUIButton ( mainPanel );
	apply = new EMUIButton ( mainPanel );

	totalSamplesLabel->SetTextColor ( LABEL_COLOR_R, LABEL_COLOR_G, LABEL_COLOR_B, LABEL_COLOR_A );
	depthLimitLabel->SetTextColor ( LABEL_COLOR_R, LABEL_COLOR_G, LABEL_COLOR_B, LABEL_COLOR_A );
	exposureLabel->SetTextColor ( LABEL_COLOR_R, LABEL_COLOR_G, LABEL_COLOR_B, LABEL_COLOR_A );

	GXLocale& locale = GXLocale::GetInstance ();

	caption->SetText ( locale.GetString ( L"Motion blur settings->Motion blur settings" ) );
	totalSamplesLabel->SetText ( locale.GetString ( L"Motion blur settings->Maximum samples" ) );
	depthLimitLabel->SetText ( locale.GetString ( L"Motion blur settings->Depth Limit (meters)" ) );
	exposureLabel->SetText ( locale.GetString ( L"Motion blur settings->Exposure (seconds)" ) );
	cancel->SetCaption ( locale.GetString ( L"Motion blur settings->Cancel" ) );
	apply->SetCaption ( locale.GetString ( L"Motion blur settings->Apply" ) );

	EMRenderer& renderer = EMRenderer::GetInstance ();
	static GXWChar buffer[ MAX_BUFER_SYMBOLS ] = { 0 };
	
	wsprintfW ( buffer, L"%hhu", renderer.GetMaxBlurSamples () );
	totalSamples->SetText ( buffer );

	wsprintfW ( buffer, L"%.6f", renderer.GetDepthLimit () );
	depthLimit->SetText ( buffer );

	wsprintfW ( buffer, L"%.6f", renderer.GetExplosureTime () );
	exposure->SetText ( buffer );

	cancel->SetOnLeftMouseButtonCallback ( this, &EMUIMotionBlurSettings::OnButton );
	apply->SetOnLeftMouseButtonCallback ( this, &EMUIMotionBlurSettings::OnButton );

	mainPanel->SetOnResizeCallback ( this, &EMUIMotionBlurSettings::OnResize );

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
	mainPanel->Hide ();
}

EMUIMotionBlurSettings::~EMUIMotionBlurSettings ()
{
	delete apply;
	delete cancel;
	delete bottomSeparator;
	delete exposure;
	delete exposureLabel;
	delete depthLimit;
	delete depthLimitLabel;
	delete totalSamples;
	delete totalSamplesLabel;
	delete topSeparator;
	delete caption;
	delete mainPanel;
}

GXWidget* EMUIMotionBlurSettings::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUIMotionBlurSettings::Show ()
{
	// TODO
}

GXVoid EMUIMotionBlurSettings::Hide ()
{
	// TODO
}

GXVoid EMUIMotionBlurSettings::ApplySettings () const
{
	// TODO
}

GXVoid GXCALL EMUIMotionBlurSettings::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	// TODO
}

GXVoid GXCALL EMUIMotionBlurSettings::OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height )
{
	// TODO
}
