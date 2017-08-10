#include <GXEngine_Editor_Mobile/EMUIToneMapperSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXUIEditBoxFloatValidator.h>
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

#define DEFAULT_MAIN_PANEL_WIDTH			6.8f
#define DEFAULT_MAIN_PANEL_HEIGHT			7.4f

#define MAIN_PANEL_MINIMUM_WIDTH			6.71469f
#define MAIN_PANEL_MINIMUM_HEIGHT			7.30558f

#define START_MAIN_PANEL_LEFT_X_OFFSET		1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET		1.5f

#define MARGIN								0.24166f

#define CAPTION_TOP_Y_OFFSET				0.64444f
#define CAPTION_HEIGHT						0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET			0.85925f
#define TOP_SEPARATOR_HEIGHT				0.2f

#define GAMMA_TOP_Y_OFFSET					1.77221f

#define EDIT_BOX_WIDTH						1.71851f
#define EDIT_BOX_HEIGHT						0.59074f

#define PROPERTY_LABEL_HEIGHT				0.59074f
#define PROPERTY_Y_OFFSET					1.12777f

#define BUTTON_WIDTH						1.90647f
#define BUTTON_HEIGHT						0.51018f
#define CANCEL_BUTTON_X_OFFSET				4.02775f

#define BOTTOM_SEPARATOR_HEIGHT				0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET	0.64444f

#define DEFAULT_FLOAT_VALIDATOR_STRING		L"1.0"

#define MINIMUM_GAMMA						1.0f
#define MAXIMUM_GAMMA						2.4f

#define MINIMUM_SENSITIVITY					0.0f
#define MAXIMUM_SENSITIVITY					777.777f

#define MINIMUM_ADOPTATION_SPEED			0.0f
#define MAXIMUM_ADOPTATION_SPEED			77.7f

#define MINIMUM_WHITE_INTENSITY				0.0f
#define MAXIMUM_WHITE_INTENSITY				77777.777f


EMUIToneMapperSettings* EMUIToneMapperSettings::instance = nullptr;

EMUIToneMapperSettings& GXCALL EMUIToneMapperSettings::GetInstance ()
{
	if ( !instance )
		instance = new EMUIToneMapperSettings ();

	return *instance;
}

EMUIToneMapperSettings::~EMUIToneMapperSettings ()
{
	delete apply;
	delete cancel;
	delete bottomSeparator;

	delete whiteIntensity->GetValidator ();
	delete whiteIntensity;
	delete whiteIntensityLabel;

	delete adoptationSpeed->GetValidator ();
	delete adoptationSpeed;
	delete adoptationSpeedLabel;

	delete sensitivity->GetValidator ();
	delete sensitivity;
	delete sensitivityLabel;

	delete gamma->GetValidator ();
	delete gamma;
	delete gammaLabel;

	delete topSeparator;
	delete caption;
	delete mainPanel;

	instance = nullptr;
}

GXWidget* EMUIToneMapperSettings::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUIToneMapperSettings::Show ()
{
	if ( !mainPanel->GetWidget ()->IsVisible () || !gamma->GetText () )
		SyncSettings ();

	mainPanel->Show ();
}

GXVoid EMUIToneMapperSettings::Hide ()
{
	mainPanel->Hide ();
}

EMUIToneMapperSettings::EMUIToneMapperSettings () :
EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( nullptr );
	caption = new EMUIStaticText ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );

	gammaLabel = new EMUIStaticText ( mainPanel );
	gamma = new EMUIEditBox ( mainPanel );
	GXUIEditBoxFloatValidator* floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *( (GXUIEditBox*)gamma->GetWidget () ), MINIMUM_GAMMA, MAXIMUM_GAMMA );
	gamma->SetValidator ( *floatValidator );

	sensitivityLabel = new EMUIStaticText ( mainPanel );
	sensitivity = new EMUIEditBox ( mainPanel );
	floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *( (GXUIEditBox*)sensitivity->GetWidget () ), MINIMUM_SENSITIVITY, MAXIMUM_SENSITIVITY );
	sensitivity->SetValidator ( *floatValidator );

	adoptationSpeedLabel = new EMUIStaticText ( mainPanel );
	adoptationSpeed = new EMUIEditBox ( mainPanel );
	floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *( (GXUIEditBox*)adoptationSpeed->GetWidget () ), MINIMUM_ADOPTATION_SPEED, MAXIMUM_ADOPTATION_SPEED );
	adoptationSpeed->SetValidator ( *floatValidator );

	whiteIntensityLabel = new EMUIStaticText ( mainPanel );
	whiteIntensity = new EMUIEditBox ( mainPanel );
	floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *( (GXUIEditBox*)adoptationSpeed->GetWidget () ), MINIMUM_WHITE_INTENSITY, MAXIMUM_WHITE_INTENSITY );
	whiteIntensity->SetValidator ( *floatValidator );

	bottomSeparator = new EMUISeparator ( mainPanel );
	cancel = new EMUIButton ( mainPanel );
	apply = new EMUIButton ( mainPanel );

	GXLocale& locale = GXLocale::GetInstance ();

	caption->SetText ( locale.GetString ( L"Tone mapper settings->HDR tone mapper settings" ) );
	caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
	caption->SetAlingment ( eGXUITextAlignment::Center );

	gammaLabel->SetText ( locale.GetString ( L"Tone mapper settings->Gamma" ) );
	gammaLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	gammaLabel->SetAlingment ( eGXUITextAlignment::Left );

	sensitivityLabel->SetText ( locale.GetString ( L"Tone mapper settings->Sensitivity" ) );
	sensitivityLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	sensitivityLabel->SetAlingment ( eGXUITextAlignment::Left );

	adoptationSpeedLabel->SetText ( locale.GetString ( L"Tone mapper settings->Adoptation speed" ) );
	adoptationSpeedLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	adoptationSpeedLabel->SetAlingment ( eGXUITextAlignment::Left );

	whiteIntensityLabel->SetText ( locale.GetString ( L"Tone mapper settings->White intensity" ) );
	whiteIntensityLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	whiteIntensityLabel->SetAlingment ( eGXUITextAlignment::Left );

	gamma->SetAlignment ( eGXUITextAlignment::Center );
	sensitivity->SetAlignment ( eGXUITextAlignment::Center );
	adoptationSpeed->SetAlignment ( eGXUITextAlignment::Center );
	whiteIntensity->SetAlignment ( eGXUITextAlignment::Center );

	cancel->SetCaption ( locale.GetString ( L"Motion blur settings->Cancel" ) );
	apply->SetCaption ( locale.GetString ( L"Motion blur settings->Apply" ) );

	cancel->SetOnLeftMouseButtonCallback ( this, &EMUIToneMapperSettings::OnButton );
	apply->SetOnLeftMouseButtonCallback ( this, &EMUIToneMapperSettings::OnButton );

	mainPanel->SetOnResizeCallback ( this, &EMUIToneMapperSettings::OnResize );

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
	mainPanel->SetMinimumWidth ( MAIN_PANEL_MINIMUM_WIDTH * gx_ui_Scale );
	mainPanel->SetMinimumHeight ( MAIN_PANEL_MINIMUM_HEIGHT * gx_ui_Scale );
	mainPanel->Hide ();
}

GXVoid EMUIToneMapperSettings::SyncSettings ()
{
	EMRenderer& renderer = EMRenderer::GetInstance ();
	static GXWChar buffer[ MAX_BUFFER_SYMBOLS ] = { 0 };

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetToneMapperGamma () );
	gamma->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetToneMapperEyeSensitivity () );
	sensitivity->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetToneMapperEyeAdaptationSpeed () );
	adoptationSpeed->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetToneMapperAbsoluteWhiteIntensity () );
	whiteIntensity->SetText ( buffer );
}

GXVoid GXCALL EMUIToneMapperSettings::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state != eGXMouseButtonState::Up ) return;

	EMUIToneMapperSettings* settings = (EMUIToneMapperSettings*)handler;

	if ( &button == settings->cancel->GetWidget () )
	{
		settings->Hide ();
		return;
	}

	EMRenderer& renderer = EMRenderer::GetInstance ();
	const GXWChar* stringW = settings->gamma->GetText ();

	if ( stringW )
	{
		GXFloat newGamma;
		GXInt result = swscanf_s ( stringW, L"%f", &newGamma );

		if ( result != 0 )
			renderer.SetToneMapperGamma ( newGamma );
	}

	stringW = settings->sensitivity->GetText ();

	if ( stringW )
	{
		GXFloat newSensitivity;
		GXInt result = swscanf_s ( stringW, L"%f", &newSensitivity );

		if ( result != 0 )
			renderer.SetToneMapperEyeSensitivity ( newSensitivity );
	}

	stringW = settings->adoptationSpeed->GetText ();

	if ( stringW )
	{
		GXFloat newAdoptationSpeed;
		GXInt result = swscanf_s ( stringW, L"%f", &newAdoptationSpeed );

		if ( result != 0 )
			renderer.SetToneMapperEyeAdaptationSpeed ( newAdoptationSpeed );
	}

	stringW = settings->whiteIntensity->GetText ();

	if ( stringW )
	{
		GXFloat newWhiteIntensity;
		GXInt result = swscanf_s ( stringW, L"%f", &newWhiteIntensity );

		if ( result != 0 )
			renderer.SetToneMapperAbsoluteWhiteIntensity ( newWhiteIntensity );
	}
}

GXVoid GXCALL EMUIToneMapperSettings::OnResize ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height )
{
	EMUIToneMapperSettings* settings = (EMUIToneMapperSettings*)handler;

	GXFloat margin = MARGIN * gx_ui_Scale;

	settings->caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );

	settings->topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat offset = height - ( MARGIN + GAMMA_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
	GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
	GXFloat editBoxXOffset = width - ( margin + editBoxWidth );
	GXFloat labelWidth = width - 2.0f * margin - editBoxWidth;
	GXFloat labelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
	GXFloat propertyYOffset = PROPERTY_Y_OFFSET * gx_ui_Scale;

	settings->gammaLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->gamma->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->sensitivityLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->sensitivity->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->adoptationSpeedLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->adoptationSpeed->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	offset -= propertyYOffset;

	settings->whiteIntensityLabel->Resize ( margin, offset, labelWidth, labelHeight );
	settings->whiteIntensity->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

	settings->bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
	GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

	settings->cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
	settings->apply->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
