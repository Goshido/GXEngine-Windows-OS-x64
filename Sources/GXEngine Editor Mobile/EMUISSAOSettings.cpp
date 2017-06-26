#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>


#define DEFAULT_MAIN_PANEL_WIDTH			8.00180f
#define DEFAULT_MAIN_PANEL_HEIGHT			7.27680f

#define START_MAIN_PANEL_LEFT_X_OFFSET		1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET		1.5f

#define CAPTION_LABEL_COLOR_R				115
#define CAPTION_LABEL_COLOR_G				185
#define CAPTION_LABEL_COLOR_B				0
#define CAPTION_LABEL_COLOR_A				255

#define PROPERTY_LABEL_COLOR_R				255
#define PROPERTY_LABEL_COLOR_G				255
#define PROPERTY_LABEL_COLOR_B				255
#define PROPERTY_LABEL_COLOR_A				255

#define MAX_BUFFER_SYMBOLS					16

#define MARGIN								0.24166f

#define CAPTION_TOP_Y_OFFSET				0.64444f
#define CAPTION_HEIGHT						0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET			0.85925f
#define TOP_SEPARATOR_HEIGHT				0.2f

#define CHECK_RADIUS_TOP_Y_OFFSET			1.77221f

#define EDIT_BOX_WIDTH						1.71851f
#define EDIT_BOX_HEIGHT						0.59074f

#define PROPERTY_LABEL_HEIGHT				0.59074f
#define PROPERTY_Y_OFFSET					1.12777f

#define BUTTON_WIDTH						1.90647f
#define BUTTON_HEIGHT						0.51018f
#define CANCEL_BUTTON_X_OFFSET				4.02775f

#define BOTTOM_SEPARATOR_HEIGHT				0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET	0.64444f


EMUISSAOSettings* EMUISSAOSettings::instance = nullptr;


EMUISSAOSettings& EMUISSAOSettings::GetInstance ()
{
	if ( !instance )
		instance = new EMUISSAOSettings ();

	return *instance;
}

EMUISSAOSettings::~EMUISSAOSettings ()
{
	delete apply;
	delete cancel;
	delete bottomSeparator;
	delete maxDistance;
	delete maxDistanceLabel;
	delete noiseTextureResolution;
	delete noiseTextureResolutionLabel;
	delete samples;
	delete samplesLabel;
	delete checkRadius;
	delete checkRadiusLabel;
	delete topSeparator;
	delete caption;
	delete mainPanel;

	instance = nullptr;
}

GXWidget* EMUISSAOSettings::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUISSAOSettings::Show ()
{
	if ( !mainPanel->GetWidget ()->IsVisible () || !checkRadius->GetText () )
		SyncSettings ();

	mainPanel->Show ();
}

GXVoid EMUISSAOSettings::Hide ()
{
	mainPanel->Hide ();
}

EMUISSAOSettings::EMUISSAOSettings () :
EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( nullptr );
	caption = new EMUIStaticText ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );
	checkRadiusLabel = new EMUIStaticText ( mainPanel );
	checkRadius = new EMUIEditBox ( mainPanel );
	samplesLabel = new EMUIStaticText ( mainPanel );
	samples = new EMUIEditBox ( mainPanel );
	noiseTextureResolutionLabel = new EMUIStaticText ( mainPanel );
	noiseTextureResolution = new EMUIEditBox ( mainPanel );
	maxDistanceLabel = new EMUIStaticText ( mainPanel );
	maxDistance = new EMUIEditBox ( mainPanel );
	bottomSeparator = new EMUISeparator ( mainPanel );
	cancel = new EMUIButton ( mainPanel );
	apply = new EMUIButton ( mainPanel );

	GXLocale& locale = GXLocale::GetInstance ();

	caption->SetText ( locale.GetString ( L"SSAO settings->SSAO settings" ) );
	caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
	caption->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );

	checkRadiusLabel->SetText ( locale.GetString ( L"SSAO settings->Check radius (meters)" ) );
	checkRadiusLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	checkRadiusLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	samplesLabel->SetText ( locale.GetString ( L"SSAO settings->Samples" ) );
	samplesLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	samplesLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	noiseTextureResolutionLabel->SetText ( locale.GetString ( L"SSAO settings->Noise texture resulotion" ) );
	noiseTextureResolutionLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	noiseTextureResolutionLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	maxDistanceLabel->SetText ( locale.GetString ( L"SSAO settings->Maximum distance (meters)" ) );
	maxDistanceLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	maxDistanceLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	checkRadius->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	samples->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	noiseTextureResolution->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	maxDistance->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );

	cancel->SetCaption ( locale.GetString ( L"SSAO settings->Cancel" ) );
	apply->SetCaption ( locale.GetString ( L"SSAO settings->Apply" ) );

	cancel->SetOnLeftMouseButtonCallback ( this, &EMUISSAOSettings::OnButton );
	apply->SetOnLeftMouseButtonCallback ( this, &EMUISSAOSettings::OnButton );

	mainPanel->SetOnResizeCallback ( this, &EMUISSAOSettings::OnResize );

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
	mainPanel->Hide ();
}

GXVoid EMUISSAOSettings::SyncSettings ()
{
	EMRenderer& renderer = EMRenderer::GetInstance ();
	static GXWChar buffer[ MAX_BUFFER_SYMBOLS ] = { 0 };

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetSSAOCheckRadius () );
	checkRadius->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", renderer.GetSSAOSampleNumber () );
	samples->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hu", renderer.GetSSAONoiseTextureResolution () );
	noiseTextureResolution->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetSSAOSSAOMaximumDistance () );
	maxDistance->SetText ( buffer );
}

GXVoid GXCALL EMUISSAOSettings::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state != eGXMouseButtonState::Up ) return;

	EMUISSAOSettings* settings = (EMUISSAOSettings*)handler;

	if ( button == settings->cancel->GetWidget () )
	{
		settings->Hide ();
		return;
	}

	EMRenderer& renderer = EMRenderer::GetInstance ();
	const GXWChar* stringW = settings->checkRadius->GetText ();

	if ( stringW )
	{
		GXFloat newCheckRadius;
		GXInt result = swscanf_s ( stringW, L"%f", &newCheckRadius );

		if ( result != 0 )
			renderer.SetSSAOCheckRadius ( newCheckRadius );
	}

	stringW = settings->samples->GetText ();

	if ( stringW )
	{
		GXUByte newSamples;
		GXInt result = swscanf_s ( stringW, L"%hhu", &newSamples );

		if ( result != 0 )
			renderer.SetSSAOSampleNumber ( newSamples );
	}

	stringW = settings->noiseTextureResolution->GetText ();

	if ( stringW )
	{
		GXUShort newNoiseTextureResolution;
		GXInt result = swscanf_s ( stringW, L"%hu", &newNoiseTextureResolution );

		if ( result != 0 )
			renderer.SetSSAONoiseTextureResolution ( newNoiseTextureResolution );
	}

	stringW = settings->maxDistance->GetText ();

	if ( stringW )
	{
		GXFloat newMaxDistance;
		GXInt result = swscanf_s ( stringW, L"%f", &newMaxDistance );

		if ( result != 0 )
			renderer.SetSSAOMaximumDistance ( newMaxDistance );
	}
}

GXVoid GXCALL EMUISSAOSettings::OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height )
{
	EMUISSAOSettings* settings = (EMUISSAOSettings*)handler;

	GXFloat margin = MARGIN * gx_ui_Scale;

	settings->caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );

	settings->topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat offset = height - ( MARGIN + CHECK_RADIUS_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
	GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
	GXFloat editBoxXOffset = width - ( margin + editBoxWidth );
	GXFloat labelWidth = width - 2.0f * margin - editBoxWidth;
	GXFloat labelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
	GXFloat propertyYOffset = PROPERTY_Y_OFFSET * gx_ui_Scale;

	settings->checkRadiusLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->checkRadius->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->samplesLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->samples->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->noiseTextureResolutionLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->noiseTextureResolution->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->maxDistanceLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->maxDistance->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	settings->bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
	GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

	settings->cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
	settings->apply->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
