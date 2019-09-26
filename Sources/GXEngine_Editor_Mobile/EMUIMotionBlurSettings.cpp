#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXUIEditBoxFloatValidator.h>
#include <GXEngine/GXUIEditBoxIntegerValidator.h>
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

#define DEFAULT_MAIN_PANEL_WIDTH            8.1f
#define DEFAULT_MAIN_PANEL_HEIGHT           6.4f

#define MAIN_PANEL_MINIMUM_WIDTH            8.02865f
#define MAIN_PANEL_MINIMUM_HEIGHT           6.39070f

#define START_MAIN_PANEL_LEFT_X_OFFSET      1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET       1.5f

#define MARGIN                              0.24166f

#define CAPTION_TOP_Y_OFFSET                0.64444f
#define CAPTION_HEIGHT                      0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET          0.85925f
#define TOP_SEPARATOR_HEIGHT                0.2f

#define MAX_SAMPLES_TOP_Y_OFFSET            1.77221f

#define EDIT_BOX_WIDTH                      1.71851f
#define EDIT_BOX_HEIGHT                     0.59074f

#define PROPERTY_LABEL_HEIGHT               0.59074f
#define PROPERTY_Y_OFFSET                   1.12777f

#define BUTTON_WIDTH                        1.90647f
#define BUTTON_HEIGHT                       0.51018f
#define CANCEL_BUTTON_X_OFFSET              4.02775f

#define BOTTOM_SEPARATOR_HEIGHT             0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET    0.64444f

#define MINIMUM_SAMPLES                     5
#define MAXIMUM_SAMPLES                     32

#define MINIMUM_DEPTH_LIMIT                 0.01f
#define MAXIMUM_DEPTH_LIMIT                 77777.7f

#define MINIMUM_EXPOSURE                    0.001f
#define MAXIMUM_EXPOSURE                    7.7f

#define DEFAULT_FLOAT_VALIDATOR_STRING      "3.0"
#define DEFAULT_INTEGER_VALIDATOR_STRING    "7"

//---------------------------------------------------------------------------------------------------------------------

EMUIMotionBlurSettings* EMUIMotionBlurSettings::_instance = nullptr;

EMUIMotionBlurSettings& EMUIMotionBlurSettings::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIMotionBlurSettings" )
        _instance = new EMUIMotionBlurSettings ();
    }

    return *_instance;
}

EMUIMotionBlurSettings::~EMUIMotionBlurSettings ()
{
    delete _apply;
    delete _cancel;
    delete _bottomSeparator;

    delete _exposure->GetValidator ();
    delete _exposure;
    delete _exposureLabel;

    delete _depthLimit->GetValidator ();
    delete _depthLimit;
    delete _depthLimitLabel;

    delete _maxSamples->GetValidator ();
    delete _maxSamples;
    delete _maxSamplesLabel;

    delete _topSeparator;
    delete _caption;

    _instance = nullptr;
}

GXWidget* EMUIMotionBlurSettings::GetWidget ()
{
    return _mainPanel.GetWidget ();
}

GXVoid EMUIMotionBlurSettings::Show ()
{
    if ( !_mainPanel.GetWidget ()->IsVisible () || _maxSamples->GetText ().IsEmpty () || _maxSamples->GetText ().IsNull () )
        SyncSettings ();

    _mainPanel.Show ();
}

GXVoid EMUIMotionBlurSettings::Hide ()
{
    _mainPanel.Hide ();
}

EMUIMotionBlurSettings::EMUIMotionBlurSettings ():
    EMUI ( nullptr ),
    _mainPanel ( nullptr )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _caption = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _topSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _maxSamplesLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _maxSamples = new EMUIEditBox ( &_mainPanel );
    GXUIEditBox* editBox = static_cast<GXUIEditBox*> ( _maxSamples->GetWidget () );
    GXUIEditBoxIntegerValidator* integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_STRING, *editBox, MINIMUM_SAMPLES, MAXIMUM_SAMPLES );
    _maxSamples->SetValidator ( *integerValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _depthLimitLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _depthLimit = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _depthLimit->GetWidget () );
    GXUIEditBoxFloatValidator* floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_DEPTH_LIMIT, MAXIMUM_DEPTH_LIMIT );
    _depthLimit->SetValidator ( *floatValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _exposureLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _exposure = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _exposure->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_EXPOSURE, MAXIMUM_EXPOSURE );
    _exposure->SetValidator ( *floatValidator );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _bottomSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _cancel = new EMUIButton ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _apply = new EMUIButton ( &_mainPanel );

    const GXLocale& locale = GXLocale::GetInstance ();

    _caption->SetText ( locale.GetString ( "Motion blur settings->Motion blur settings" ) );
    _caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
    _caption->SetAlingment ( eGXUITextAlignment::Center );

    _maxSamplesLabel->SetText ( locale.GetString ( "Motion blur settings->Maximum samples" ) );
    _maxSamplesLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _maxSamplesLabel->SetAlingment ( eGXUITextAlignment::Left );

    _depthLimitLabel->SetText ( locale.GetString ( "Motion blur settings->Depth Limit (meters)" ) );
    _depthLimitLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _depthLimitLabel->SetAlingment ( eGXUITextAlignment::Left );

    _exposureLabel->SetText ( locale.GetString ( "Motion blur settings->Exposure (seconds)" ) );
    _exposureLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _exposureLabel->SetAlingment ( eGXUITextAlignment::Left );

    _maxSamples->SetAlignment ( eGXUITextAlignment::Center );
    _depthLimit->SetAlignment ( eGXUITextAlignment::Center );
    _exposure->SetAlignment ( eGXUITextAlignment::Center );

    _cancel->SetCaption ( locale.GetString ( "Motion blur settings->Cancel" ) );
    _apply->SetCaption ( locale.GetString ( "Motion blur settings->Apply" ) );

    _cancel->SetOnLeftMouseButtonCallback ( this, &EMUIMotionBlurSettings::OnButton );
    _apply->SetOnLeftMouseButtonCallback ( this, &EMUIMotionBlurSettings::OnButton );

    _mainPanel.SetOnResizeCallback ( this, &EMUIMotionBlurSettings::OnResize );

    const GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
    _mainPanel.Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
    _mainPanel.SetMinimumWidth ( MAIN_PANEL_MINIMUM_WIDTH * gx_ui_Scale );
    _mainPanel.SetMinimumHeight ( MAIN_PANEL_MINIMUM_HEIGHT * gx_ui_Scale );
    _mainPanel.Hide ();
}

GXVoid EMUIMotionBlurSettings::SyncSettings ()
{
    const EMRenderer& renderer = EMRenderer::GetInstance ();

    _buffer.Format ( "%hu", static_cast<GXShort> ( renderer.GetMaximumMotionBlurSamples () ) );
    _maxSamples->SetText ( _buffer );

    _buffer.Format ( "%.6g", renderer.GetMotionBlurDepthLimit () );
    _depthLimit->SetText ( _buffer );

    _buffer.Format ( "%.6g", renderer.GetMotionBlurExposure () );
    _exposure->SetText ( _buffer );
}

GXVoid GXCALL EMUIMotionBlurSettings::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
    if ( state != eGXMouseButtonState::Up ) return;

    EMUIMotionBlurSettings* settings = (EMUIMotionBlurSettings*)handler;

    if ( &button == settings->_cancel->GetWidget () )
    {
        settings->Hide ();
        return;
    }

    EMRenderer& renderer = EMRenderer::GetInstance ();
    const GXWChar* stringW = settings->_maxSamples->GetText ();

    if ( stringW )
    {
        GXUShort newMaxSamples;
        const GXInt result = swscanf_s ( stringW, L"%hu", &newMaxSamples );

        if ( result != 0 )
        {
            renderer.SetMaximumMotionBlurSamples ( static_cast<GXUByte> ( newMaxSamples ) );
        }
    }

    stringW = settings->_depthLimit->GetText ();

    if ( stringW )
    {
        GXFloat newDepthLimit;
        const GXInt result = swscanf_s ( stringW, L"%f", &newDepthLimit );

        if ( result != 0 )
        {
            renderer.SetMotionBlurDepthLimit ( newDepthLimit );
        }
    }

    stringW = settings->_exposure->GetText ();

    if ( !stringW ) return;

    GXFloat newExposure;
    const GXInt result = swscanf_s ( stringW, L"%f", &newExposure );

    if ( result == 0 ) return;

    renderer.SetMotionBlurExposure ( newExposure );
}

GXVoid GXCALL EMUIMotionBlurSettings::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
    EMUIMotionBlurSettings* settings = static_cast<EMUIMotionBlurSettings*> ( handler );

    const GXFloat margin = MARGIN * gx_ui_Scale;

    settings->_caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );
    
    settings->_topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );

    GXFloat offset = height - ( MARGIN + MAX_SAMPLES_TOP_Y_OFFSET ) * gx_ui_Scale;
    const GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
    const GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
    const GXFloat editBoxXOffset = width - ( margin + editBoxWidth );
    const GXFloat labelWidth = width - 2.0f * margin - editBoxWidth;
    const GXFloat labelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
    const GXFloat propertyYOffset = PROPERTY_Y_OFFSET * gx_ui_Scale;

    settings->_maxSamplesLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_maxSamples->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_depthLimitLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_depthLimit->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_exposureLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_exposure->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    settings->_bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

    const GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
    const GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

    settings->_cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
    settings->_apply->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
