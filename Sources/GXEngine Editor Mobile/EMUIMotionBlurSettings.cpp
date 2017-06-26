#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXRenderer.h>


#define CAPTION_LABEL_COLOR_R				115
#define CAPTION_LABEL_COLOR_G				185
#define CAPTION_LABEL_COLOR_B				0
#define CAPTION_LABEL_COLOR_A				255

#define PROPERTY_LABEL_COLOR_R				255
#define PROPERTY_LABEL_COLOR_G				255
#define PROPERTY_LABEL_COLOR_B				255
#define PROPERTY_LABEL_COLOR_A				255

#define MAX_BUFFER_SYMBOLS					16

#define DEFAULT_MAIN_PANEL_WIDTH			8.02865f
#define DEFAULT_MAIN_PANEL_HEIGHT			6.39070f

#define START_MAIN_PANEL_LEFT_X_OFFSET		1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET		1.5f

#define CAPTION_TOP_Y_OFFSET				0.64444f
#define CAPTION_HEIGHT						0.5f

#define MARGIN								0.24166f

#define TOP_SEPARATOR_TOP_Y_OFFSET			0.85925f
#define TOP_SEPARATOR_HEIGHT				0.2f

#define MAX_SAMPLES_TOP_Y_OFFSET			1.77221f

#define EDIT_BOX_WIDTH						1.71851f
#define EDIT_BOX_HEIGHT						0.59074f

#define PROPERTY_LABEL_HEIGHT				0.59074f
#define PROPERTY_Y_OFFSET					1.12777f

#define BUTTON_WIDTH						1.90647f
#define BUTTON_HEIGHT						0.51018f
#define CANCEL_BUTTON_X_OFFSET				4.02775f

#define BOTTOM_SEPARATOR_HEIGHT				0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET	0.64444f


EMUIMotionBlurSettings* EMUIMotionBlurSettings::instance = nullptr;

EMUIMotionBlurSettings& EMUIMotionBlurSettings::GetInstance ()
{
	if ( !instance )
		instance = new EMUIMotionBlurSettings ();

	return *instance;
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
	delete maxSamples;
	delete maxSamplesLabel;
	delete topSeparator;
	delete caption;
	delete mainPanel;

	instance = nullptr;
}

GXWidget* EMUIMotionBlurSettings::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUIMotionBlurSettings::Show ()
{
	if ( !mainPanel->GetWidget ()->IsVisible () || !maxSamples->GetText () )
		SyncSettings ();

	mainPanel->Show ();
}

GXVoid EMUIMotionBlurSettings::Hide ()
{
	mainPanel->Hide ();
}

EMUIMotionBlurSettings::EMUIMotionBlurSettings () :
	EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( nullptr );
	caption = new EMUIStaticText ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );
	maxSamplesLabel = new EMUIStaticText ( mainPanel );
	maxSamples = new EMUIEditBox ( mainPanel );
	depthLimitLabel = new EMUIStaticText ( mainPanel );
	depthLimit = new EMUIEditBox ( mainPanel );
	exposureLabel = new EMUIStaticText ( mainPanel );
	exposure = new EMUIEditBox ( mainPanel );
	bottomSeparator = new EMUISeparator ( mainPanel );
	cancel = new EMUIButton ( mainPanel );
	apply = new EMUIButton ( mainPanel );

	GXLocale& locale = GXLocale::GetInstance ();

	caption->SetText ( locale.GetString ( L"Motion blur settings->Motion blur settings" ) );
	caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
	caption->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );

	maxSamplesLabel->SetText ( locale.GetString ( L"Motion blur settings->Maximum samples" ) );
	maxSamplesLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	maxSamplesLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	depthLimitLabel->SetText ( locale.GetString ( L"Motion blur settings->Depth Limit (meters)" ) );
	depthLimitLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	depthLimitLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	exposureLabel->SetText ( locale.GetString ( L"Motion blur settings->Exposure (seconds)" ) );
	exposureLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	exposureLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	maxSamples->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	depthLimit->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	exposure->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );

	cancel->SetCaption ( locale.GetString ( L"Motion blur settings->Cancel" ) );
	apply->SetCaption ( locale.GetString ( L"Motion blur settings->Apply" ) );

	cancel->SetOnLeftMouseButtonCallback ( this, &EMUIMotionBlurSettings::OnButton );
	apply->SetOnLeftMouseButtonCallback ( this, &EMUIMotionBlurSettings::OnButton );

	mainPanel->SetOnResizeCallback ( this, &EMUIMotionBlurSettings::OnResize );

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
	mainPanel->Hide ();
}

GXVoid EMUIMotionBlurSettings::SyncSettings ()
{
	EMRenderer& renderer = EMRenderer::GetInstance ();
	static GXWChar buffer[ MAX_BUFFER_SYMBOLS ] = { 0 };

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", renderer.GetMaximumMotionBlurSamples () );
	maxSamples->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetMotionBlurDepthLimit () );
	depthLimit->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetMotionBlurExplosure () );
	exposure->SetText ( buffer );
}

GXVoid GXCALL EMUIMotionBlurSettings::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
	if ( state != eGXMouseButtonState::Up ) return;

	EMUIMotionBlurSettings* settings = (EMUIMotionBlurSettings*)handler;

	if ( button == settings->cancel->GetWidget () )
	{
		settings->Hide ();
		return;
	}

	EMRenderer& renderer = EMRenderer::GetInstance ();
	const GXWChar* stringW = settings->maxSamples->GetText ();

	if ( stringW )
	{
		GXUByte newMaxSamples;
		GXInt result = swscanf_s ( stringW, L"%hhu", &newMaxSamples );

		if ( result != 0 )
			renderer.SetMaximumMotionBlurSamples ( newMaxSamples );
	}

	stringW = settings->depthLimit->GetText ();

	if ( stringW )
	{
		GXFloat newDepthLimit;
		GXInt result = swscanf_s ( stringW, L"%f", &newDepthLimit );

		if ( result != 0 )
			renderer.SetMotionBlurDepthLimit ( newDepthLimit );
	}

	stringW = settings->exposure->GetText ();

	if ( stringW )
	{
		GXFloat newExposure;
		GXInt result = swscanf_s ( stringW, L"%f", &newExposure );

		if ( result != 0 )
			renderer.SetMotionBlurExposure ( newExposure );
	}
}

GXVoid GXCALL EMUIMotionBlurSettings::OnResize ( GXVoid* handler, GXUIDragableArea* /*area*/, GXFloat width, GXFloat height )
{
	EMUIMotionBlurSettings* settings = (EMUIMotionBlurSettings*)handler;

	GXFloat margin = MARGIN * gx_ui_Scale;

	settings->caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );
	
	settings->topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat offset = height - ( MARGIN + MAX_SAMPLES_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
	GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
	GXFloat editBoxXOffset = width - ( margin + editBoxWidth );
	GXFloat labelWidth = width - 2.0f * margin - editBoxWidth;
	GXFloat labelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
	GXFloat propertyYOffset = PROPERTY_Y_OFFSET * gx_ui_Scale;

	settings->maxSamplesLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->maxSamples->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->depthLimitLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->depthLimit->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->exposureLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->exposure->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	settings->bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
	GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

	settings->cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
	settings->apply->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
