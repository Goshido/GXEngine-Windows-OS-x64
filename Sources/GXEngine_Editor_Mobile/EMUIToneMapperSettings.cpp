#include <GXEngine_Editor_Mobile/EMUIToneMapperSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXUIEditBoxFloatValidator.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXRenderer.h>


#define CAPTION_LABEL_COLOR_R               115u
#define CAPTION_LABEL_COLOR_G               185u
#define CAPTION_LABEL_COLOR_B               0u
#define CAPTION_LABEL_COLOR_A               255u

#define PROPERTY_LABEL_COLOR_R              255u
#define PROPERTY_LABEL_COLOR_G              255u
#define PROPERTY_LABEL_COLOR_B              255u
#define PROPERTY_LABEL_COLOR_A              255u

#define MAX_BUFFER_SYMBOLS                  16u

#define DEFAULT_MAIN_PANEL_WIDTH            6.8f
#define DEFAULT_MAIN_PANEL_HEIGHT           7.4f

#define MAIN_PANEL_MINIMUM_WIDTH            6.71469f
#define MAIN_PANEL_MINIMUM_HEIGHT           7.30558f

#define START_MAIN_PANEL_LEFT_X_OFFSET      1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET       1.5f

#define MARGIN                              0.24166f

#define CAPTION_TOP_Y_OFFSET                0.64444f
#define CAPTION_HEIGHT                      0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET          0.85925f
#define TOP_SEPARATOR_HEIGHT                0.2f

#define GAMMA_TOP_Y_OFFSET                  1.77221f

#define EDIT_BOX_WIDTH                      1.71851f
#define EDIT_BOX_HEIGHT                     0.59074f

#define PROPERTY_LABEL_HEIGHT               0.59074f
#define PROPERTY_Y_OFFSET                   1.12777f

#define BUTTON_WIDTH                        1.90647f
#define BUTTON_HEIGHT                       0.51018f
#define CANCEL_BUTTON_X_OFFSET              4.02775f

#define BOTTOM_SEPARATOR_HEIGHT             0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET    0.64444f

#define DEFAULT_FLOAT_VALIDATOR_STRING      "1.0"

#define MINIMUM_GAMMA                       1.0f
#define MAXIMUM_GAMMA                       2.4f

#define MINIMUM_SENSITIVITY                 0.0f
#define MAXIMUM_SENSITIVITY                 777.777f

#define MINIMUM_ADOPTATION_SPEED            0.0f
#define MAXIMUM_ADOPTATION_SPEED            77.7f

#define MINIMUM_WHITE_INTENSITY             0.0f
#define MAXIMUM_WHITE_INTENSITY             77777.777f

//---------------------------------------------------------------------------------------------------------------------

EMUIToneMapperSettings* EMUIToneMapperSettings::_instance = nullptr;

EMUIToneMapperSettings& GXCALL EMUIToneMapperSettings::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIToneMapperSettings" )
        _instance = new EMUIToneMapperSettings ();
    }

    return *_instance;
}

EMUIToneMapperSettings::~EMUIToneMapperSettings ()
{
    delete _apply;
    delete _cancel;
    delete _bottomSeparator;

    delete _whiteIntensity->GetValidator ();
    delete _whiteIntensity;
    delete _whiteIntensityLabel;

    delete _eyeAdaptationSpeed->GetValidator ();
    delete _eyeAdaptationSpeed;
    delete _eyeAdaptationSpeedLabel;

    delete _sensitivity->GetValidator ();
    delete _sensitivity;
    delete _sensitivityLabel;

    delete _gamma->GetValidator ();
    delete _gamma;
    delete _gammaLabel;

    delete _topSeparator;
    delete _caption;


    _instance = nullptr;
}

GXWidget* EMUIToneMapperSettings::GetWidget ()
{
    return _mainPanel.GetWidget ();
}

GXVoid EMUIToneMapperSettings::Show ()
{
    if ( !_mainPanel.GetWidget ()->IsVisible () || _gamma->GetText ().IsNull () || _gamma->GetText ().IsEmpty () )
        SyncSettings ();

    _mainPanel.Show ();
}

GXVoid EMUIToneMapperSettings::Hide ()
{
    _mainPanel.Hide ();
}

EMUIToneMapperSettings::EMUIToneMapperSettings ():
    EMUI ( nullptr ),
    _mainPanel ( nullptr )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _caption = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _topSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _gammaLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _gamma = new EMUIEditBox ( &_mainPanel );
    GXUIEditBox* editBox = static_cast<GXUIEditBox*> ( _gamma->GetWidget () );
    GXUIEditBoxFloatValidator* floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_GAMMA, MAXIMUM_GAMMA );
    _gamma->SetValidator ( *floatValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _sensitivityLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _sensitivity = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _sensitivity->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_SENSITIVITY, MAXIMUM_SENSITIVITY );
    _sensitivity->SetValidator ( *floatValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _eyeAdaptationSpeedLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _eyeAdaptationSpeed = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _eyeAdaptationSpeed->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_ADOPTATION_SPEED, MAXIMUM_ADOPTATION_SPEED );
    _eyeAdaptationSpeed->SetValidator ( *floatValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _whiteIntensityLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _whiteIntensity = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _whiteIntensity->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_WHITE_INTENSITY, MAXIMUM_WHITE_INTENSITY );
    _whiteIntensity->SetValidator ( *floatValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _bottomSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _cancel = new EMUIButton ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _apply = new EMUIButton ( &_mainPanel );

    const GXLocale& locale = GXLocale::GetInstance ();

    _caption->SetText ( locale.GetString ( "Tone mapper settings->HDR tone mapper settings" ) );
    _caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
    _caption->SetAlingment ( eGXUITextAlignment::Center );

    _gammaLabel->SetText ( locale.GetString ( "Tone mapper settings->Gamma" ) );
    _gammaLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _gammaLabel->SetAlingment ( eGXUITextAlignment::Left );

    _sensitivityLabel->SetText ( locale.GetString ( "Tone mapper settings->Sensitivity" ) );
    _sensitivityLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _sensitivityLabel->SetAlingment ( eGXUITextAlignment::Left );

    _eyeAdaptationSpeedLabel->SetText ( locale.GetString ( "Tone mapper settings->Eye adaptation speed" ) );
    _eyeAdaptationSpeedLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _eyeAdaptationSpeedLabel->SetAlingment ( eGXUITextAlignment::Left );

    _whiteIntensityLabel->SetText ( locale.GetString ( "Tone mapper settings->White intensity" ) );
    _whiteIntensityLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _whiteIntensityLabel->SetAlingment ( eGXUITextAlignment::Left );

    _gamma->SetAlignment ( eGXUITextAlignment::Center );
    _sensitivity->SetAlignment ( eGXUITextAlignment::Center );
    _eyeAdaptationSpeed->SetAlignment ( eGXUITextAlignment::Center );
    _whiteIntensity->SetAlignment ( eGXUITextAlignment::Center );

    _cancel->SetCaption ( locale.GetString ( "Motion blur settings->Cancel" ) );
    _apply->SetCaption ( locale.GetString ( "Motion blur settings->Apply" ) );

    _cancel->SetOnLeftMouseButtonCallback ( this, &EMUIToneMapperSettings::OnButton );
    _apply->SetOnLeftMouseButtonCallback ( this, &EMUIToneMapperSettings::OnButton );

    _mainPanel.SetOnResizeCallback ( this, &EMUIToneMapperSettings::OnResize );

    const GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
    _mainPanel.Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
    _mainPanel.SetMinimumWidth ( MAIN_PANEL_MINIMUM_WIDTH * gx_ui_Scale );
    _mainPanel.SetMinimumHeight ( MAIN_PANEL_MINIMUM_HEIGHT * gx_ui_Scale );
    _mainPanel.Hide ();
}

GXVoid EMUIToneMapperSettings::SyncSettings ()
{
    EMRenderer& renderer = EMRenderer::GetInstance ();

    _buffer.Format ( "%.6g", renderer.GetToneMapperGamma () );
    _gamma->SetText ( _buffer );

    _buffer.Format ( "%.6g", renderer.GetToneMapperEyeSensitivity () );
    _sensitivity->SetText ( _buffer );

    _buffer.Format ( "%.6g", renderer.GetToneMapperEyeAdaptationSpeed () );
    _eyeAdaptationSpeed->SetText ( _buffer );

    _buffer.Format ( "%.6g", renderer.GetToneMapperAbsoluteWhiteIntensity () );
    _whiteIntensity->SetText ( _buffer );
}

GXVoid GXCALL EMUIToneMapperSettings::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
    if ( state != eGXMouseButtonState::Up ) return;

    EMUIToneMapperSettings* settings = static_cast<EMUIToneMapperSettings*> ( handler );

    if ( &button == settings->_cancel->GetWidget () )
    {
        settings->Hide ();
        return;
    }

    EMRenderer& renderer = EMRenderer::GetInstance ();
    const GXWChar* stringW = settings->_gamma->GetText ();

    if ( stringW )
    {
        GXFloat newGamma;
        const GXInt result = swscanf_s ( stringW, L"%f", &newGamma );

        if ( result != 0 )
        {
            renderer.SetToneMapperGamma ( newGamma );
        }
    }

    stringW = settings->_sensitivity->GetText ();

    if ( stringW )
    {
        GXFloat newSensitivity;
        const GXInt result = swscanf_s ( stringW, L"%f", &newSensitivity );

        if ( result != 0 )
        {
            renderer.SetToneMapperEyeSensitivity ( newSensitivity );
        }
    }

    stringW = settings->_eyeAdaptationSpeed->GetText ();

    if ( stringW )
    {
        GXFloat newEyeAdaptationSpeed;
        const GXInt result = swscanf_s ( stringW, L"%f", &newEyeAdaptationSpeed );

        if ( result != 0 )
        {
            renderer.SetToneMapperEyeAdaptationSpeed ( newEyeAdaptationSpeed );
        }
    }

    stringW = settings->_whiteIntensity->GetText ();

    if ( !stringW ) return;

    GXFloat newWhiteIntensity;
    const GXInt result = swscanf_s ( stringW, L"%f", &newWhiteIntensity );

    if ( result == 0 ) return;

    renderer.SetToneMapperAbsoluteWhiteIntensity ( newWhiteIntensity );
}

GXVoid GXCALL EMUIToneMapperSettings::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
    EMUIToneMapperSettings* settings = static_cast<EMUIToneMapperSettings*> ( handler );

    const GXFloat margin = MARGIN * gx_ui_Scale;

    settings->_caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );

    settings->_topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );

    GXFloat offset = height - ( MARGIN + GAMMA_TOP_Y_OFFSET ) * gx_ui_Scale;
    const GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
    const GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
    const GXFloat editBoxXOffset = width - ( margin + editBoxWidth );
    const GXFloat labelWidth = width - 2.0f * margin - editBoxWidth;
    const GXFloat labelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
    const GXFloat propertyYOffset = PROPERTY_Y_OFFSET * gx_ui_Scale;

    settings->_gammaLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_gamma->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_sensitivityLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_sensitivity->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_eyeAdaptationSpeedLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_eyeAdaptationSpeed->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_whiteIntensityLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_whiteIntensity->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    settings->_bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

    const GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
    const GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

    settings->_cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
    settings->_apply->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
