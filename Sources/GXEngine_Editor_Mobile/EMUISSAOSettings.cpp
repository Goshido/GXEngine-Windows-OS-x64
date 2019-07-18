#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXUIEditBoxIntegerValidator.h>
#include <GXEngine/GXUIEditBoxFloatValidator.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>


#define DEFAULT_MAIN_PANEL_WIDTH            8.1f
#define DEFAULT_MAIN_PANEL_HEIGHT           7.3f

#define MAIN_PANEL_MINIMUM_WIDTH            8.00180f
#define MAIN_PANEL_MINIMUM_HEIGHT           7.27680f

#define START_MAIN_PANEL_LEFT_X_OFFSET      1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET       1.5f

#define CAPTION_LABEL_COLOR_R               115u
#define CAPTION_LABEL_COLOR_G               185u
#define CAPTION_LABEL_COLOR_B               0u
#define CAPTION_LABEL_COLOR_A               255u

#define PROPERTY_LABEL_COLOR_R              255u
#define PROPERTY_LABEL_COLOR_G              255u
#define PROPERTY_LABEL_COLOR_B              255u
#define PROPERTY_LABEL_COLOR_A              255u

#define MAX_BUFFER_SYMBOLS                  16u

#define MARGIN                              0.24166f

#define CAPTION_TOP_Y_OFFSET                0.64444f
#define CAPTION_HEIGHT                      0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET          0.85925f
#define TOP_SEPARATOR_HEIGHT                0.2f

#define CHECK_RADIUS_TOP_Y_OFFSET           1.77221f

#define EDIT_BOX_WIDTH                      1.71851f
#define EDIT_BOX_HEIGHT                     0.59074f

#define PROPERTY_LABEL_HEIGHT               0.59074f
#define PROPERTY_Y_OFFSET                   1.12777f

#define BUTTON_WIDTH                        1.90647f
#define BUTTON_HEIGHT                       0.51018f
#define CANCEL_BUTTON_X_OFFSET              4.02775f

#define BOTTOM_SEPARATOR_HEIGHT             0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET    0.64444f

#define DEFAULT_INTEGER_VALIDATOR_STRING    L"4"
#define DEFAULT_FLOAT_VALIDATOR_STRING      L"0.3"

#define MINIMUM_CHECK_RADIUS                0.01f
#define MAXIMUM_CHECK_RADIUS                77777.7f

#define MINIMUM_SAMPLES                     4u
#define MAXIMUM_SAMPLES                     64u

#define MINIMUM_NOISE_TEXTURE_RESOLUTION    2u
#define MAXIMUM_NOISE_TEXTURE_RESOLUTION    4096u

#define MINIMUM_DISTANCE                    0.3f
#define MAXIMUM_DISTANCE                    77777.7f

//---------------------------------------------------------------------------------------------------------------------

EMUISSAOSettings* EMUISSAOSettings::_instance = nullptr;

EMUISSAOSettings& EMUISSAOSettings::GetInstance ()
{
    if ( !_instance )
        _instance = new EMUISSAOSettings ();

    return *_instance;
}

EMUISSAOSettings::~EMUISSAOSettings ()
{
    delete _apply;
    delete _cancel;
    delete _bottomSeparator;

    delete _maxDistance->GetValidator ();
    delete _maxDistance;
    delete _maxDistanceLabel;

    delete _noiseTextureResolution->GetValidator ();
    delete _noiseTextureResolution;
    delete _noiseTextureResolutionLabel;

    delete _samples->GetValidator ();
    delete _samples;
    delete _samplesLabel;

    delete _checkRadius->GetValidator ();
    delete _checkRadius;
    delete _checkRadiusLabel;

    delete _topSeparator;
    delete _caption;
    delete _mainPanel;

    _instance = nullptr;
}

GXWidget* EMUISSAOSettings::GetWidget () const
{
    return _mainPanel->GetWidget ();
}

GXVoid EMUISSAOSettings::Show ()
{
    if ( !_mainPanel->GetWidget ()->IsVisible () || !_checkRadius->GetText () )
        SyncSettings ();

    _mainPanel->Show ();
}

GXVoid EMUISSAOSettings::Hide ()
{
    _mainPanel->Hide ();
}

EMUISSAOSettings::EMUISSAOSettings ():
    EMUI ( nullptr ),
    _mainPanel ( new EMUIDraggableArea ( nullptr ) )
{
    _caption = new EMUIStaticText ( _mainPanel );
    _topSeparator = new EMUISeparator ( _mainPanel );

    _checkRadiusLabel = new EMUIStaticText ( _mainPanel );
    _checkRadius = new EMUIEditBox ( _mainPanel );
    GXUIEditBox* editBox = static_cast<GXUIEditBox*> ( _checkRadius->GetWidget () );
    GXUIEditBoxFloatValidator* floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_CHECK_RADIUS, MAXIMUM_CHECK_RADIUS );
    _checkRadius->SetValidator ( *floatValidator );

    _samplesLabel = new EMUIStaticText ( _mainPanel );
    _samples = new EMUIEditBox ( _mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _samples->GetWidget () );
    GXUIEditBoxIntegerValidator* integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_STRING, *editBox, MINIMUM_SAMPLES, MAXIMUM_SAMPLES );
    _samples->SetValidator ( *integerValidator );

    _noiseTextureResolutionLabel = new EMUIStaticText ( _mainPanel );
    _noiseTextureResolution = new EMUIEditBox ( _mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _noiseTextureResolution->GetWidget () );
    integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_STRING, *editBox, MINIMUM_NOISE_TEXTURE_RESOLUTION, MAXIMUM_NOISE_TEXTURE_RESOLUTION );
    _noiseTextureResolution->SetValidator ( *integerValidator );

    _maxDistanceLabel = new EMUIStaticText ( _mainPanel );
    _maxDistance = new EMUIEditBox ( _mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _maxDistance->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_STRING, *editBox, MINIMUM_DISTANCE, MAXIMUM_DISTANCE );
    _maxDistance->SetValidator ( *floatValidator );

    _bottomSeparator = new EMUISeparator ( _mainPanel );
    _cancel = new EMUIButton ( _mainPanel );
    _apply = new EMUIButton ( _mainPanel );

    const GXLocale& locale = GXLocale::GetInstance ();

    _caption->SetText ( locale.GetString ( L"SSAO settings->SSAO settings" ) );
    _caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
    _caption->SetAlingment ( eGXUITextAlignment::Center );

    _checkRadiusLabel->SetText ( locale.GetString ( L"SSAO settings->Check radius (meters)" ) );
    _checkRadiusLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _checkRadiusLabel->SetAlingment ( eGXUITextAlignment::Left );

    _samplesLabel->SetText ( locale.GetString ( L"SSAO settings->Samples" ) );
    _samplesLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _samplesLabel->SetAlingment ( eGXUITextAlignment::Left );

    _noiseTextureResolutionLabel->SetText ( locale.GetString ( L"SSAO settings->Noise texture resulotion" ) );
    _noiseTextureResolutionLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _noiseTextureResolutionLabel->SetAlingment ( eGXUITextAlignment::Left );

    _maxDistanceLabel->SetText ( locale.GetString ( L"SSAO settings->Maximum distance (meters)" ) );
    _maxDistanceLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
    _maxDistanceLabel->SetAlingment ( eGXUITextAlignment::Left );

    _checkRadius->SetAlignment ( eGXUITextAlignment::Center );
    _samples->SetAlignment ( eGXUITextAlignment::Center );
    _noiseTextureResolution->SetAlignment ( eGXUITextAlignment::Center );
    _maxDistance->SetAlignment ( eGXUITextAlignment::Center );

    _cancel->SetCaption ( locale.GetString ( L"SSAO settings->Cancel" ) );
    _apply->SetCaption ( locale.GetString ( L"SSAO settings->Apply" ) );

    _cancel->SetOnLeftMouseButtonCallback ( this, &EMUISSAOSettings::OnButton );
    _apply->SetOnLeftMouseButtonCallback ( this, &EMUISSAOSettings::OnButton );

    _mainPanel->SetOnResizeCallback ( this, &EMUISSAOSettings::OnResize );

    const GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
    _mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
    _mainPanel->SetMinimumWidth ( MAIN_PANEL_MINIMUM_WIDTH * gx_ui_Scale );
    _mainPanel->SetMinimumHeight ( MAIN_PANEL_MINIMUM_HEIGHT * gx_ui_Scale );
    _mainPanel->Hide ();
}

GXVoid EMUISSAOSettings::SyncSettings ()
{
    EMRenderer& renderer = EMRenderer::GetInstance ();
    GXWChar buffer[ MAX_BUFFER_SYMBOLS ];

    swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetSSAOCheckRadius () );
    _checkRadius->SetText ( buffer );

    swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", renderer.GetSSAOSampleNumber () );
    _samples->SetText ( buffer );

    swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hu", renderer.GetSSAONoiseTextureResolution () );
    _noiseTextureResolution->SetText ( buffer );

    swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", renderer.GetSSAOMaximumDistance () );
    _maxDistance->SetText ( buffer );
}

GXVoid GXCALL EMUISSAOSettings::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
    if ( state != eGXMouseButtonState::Up ) return;

    EMUISSAOSettings* settings = static_cast<EMUISSAOSettings*> ( handler );

    if ( &button == settings->_cancel->GetWidget () )
    {
        settings->Hide ();
        return;
    }

    EMRenderer& renderer = EMRenderer::GetInstance ();
    const GXWChar* stringW = settings->_checkRadius->GetText ();

    if ( stringW )
    {
        GXFloat newCheckRadius;
        const GXInt result = swscanf_s ( stringW, L"%f", &newCheckRadius );

        if ( result != 0 )
        {
            renderer.SetSSAOCheckRadius ( newCheckRadius );
        }
    }

    stringW = settings->_samples->GetText ();

    if ( stringW )
    {
        GXUByte newSamples;
        const GXInt result = swscanf_s ( stringW, L"%hhu", &newSamples );

        if ( result != 0 )
        {
            renderer.SetSSAOSampleNumber ( newSamples );
        }
    }

    stringW = settings->_noiseTextureResolution->GetText ();

    if ( stringW )
    {
        GXUShort newNoiseTextureResolution;
        const GXInt result = swscanf_s ( stringW, L"%hu", &newNoiseTextureResolution );

        if ( result != 0 )
        {
            renderer.SetSSAONoiseTextureResolution ( newNoiseTextureResolution );
        }
    }

    stringW = settings->_maxDistance->GetText ();

    if ( !stringW ) return;

    GXFloat newMaxDistance;
    const GXInt result = swscanf_s ( stringW, L"%f", &newMaxDistance );

    if ( result == 0 ) return;

    renderer.SetSSAOMaximumDistance ( newMaxDistance );
}

GXVoid GXCALL EMUISSAOSettings::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
    EMUISSAOSettings* settings = static_cast<EMUISSAOSettings*> ( handler );

    const GXFloat margin = MARGIN * gx_ui_Scale;

    settings->_caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );

    settings->_topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );

    GXFloat offset = height - ( MARGIN + CHECK_RADIUS_TOP_Y_OFFSET ) * gx_ui_Scale;
    const GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
    const GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
    const GXFloat editBoxXOffset = width - ( margin + editBoxWidth );
    const GXFloat labelWidth = width - 2.0f * margin - editBoxWidth;
    const GXFloat labelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
    const GXFloat propertyYOffset = PROPERTY_Y_OFFSET * gx_ui_Scale;

    settings->_checkRadiusLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_checkRadius->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_samplesLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_samples->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_noiseTextureResolutionLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_noiseTextureResolution->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    offset -= propertyYOffset;

    settings->_maxDistanceLabel->Resize ( margin, offset, labelWidth, labelHeight );
    settings->_maxDistance->Resize ( editBoxXOffset, offset, editBoxWidth, editBoxHeight );

    settings->_bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

    const GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
    const GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

    settings->_cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
    settings->_apply->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
