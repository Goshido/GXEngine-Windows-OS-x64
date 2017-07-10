#include <GXEngine_Editor_Mobile/EMUIColorPicker.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define CAPTION_LABEL_COLOR_R						115
#define CAPTION_LABEL_COLOR_G						185
#define CAPTION_LABEL_COLOR_B						0
#define CAPTION_LABEL_COLOR_A						255

#define PROPERTY_LABEL_COLOR_R						255
#define PROPERTY_LABEL_COLOR_G						255
#define PROPERTY_LABEL_COLOR_B						255
#define PROPERTY_LABEL_COLOR_A						255

#define DEFAULT_MAIN_PANEL_WIDTH					6.71292f
#define DEFAULT_MAIN_PANEL_HEIGHT					16.19156f

#define START_MAIN_PANEL_LEFT_X_OFFSET				1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET				1.5f

#define MARGIN										0.24166f

#define CAPTION_TOP_Y_OFFSET						0.64444f
#define CAPTION_HEIGHT								0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET					0.85925f
#define TOP_SEPARATOR_HEIGHT						0.2f

#define HSV_COLOR_WIDGET_TOP_Y_OFFSET				7.41106f
#define HSV_COLOR_WIDGET_HEIGHT						6.25644f

#define COLOR_SEPARATOR_TOP_Y_OFFSET				8.24346f
#define COLOR_SEPARATOR_HEIGHT						0.53703f

#define ADD_COLOR_BUTTON_WIDTH						0.59074f
#define ADD_COLOR_BUTTON_HEIGHT						0.59074f
#define ADD_COLOR_BUTTON_TOP_Y_OFFSET				9.29068f

#define SAVED_COLOR_WIDTH							0.53703f
#define SAVED_COLOR_HEIGHT							0.53703f
#define SAVED_COLOR_RIGHT_OFFSET					5.31663f
#define SAVED_COLOR_HORIZONTAL_STEP					0.69814f
#define SAVED_COLOR_TOP_ROW_TOP_Y_OFFSET			9.26383f
#define SAVED_COLOR_BOTTOM_ROW_TOP_Y_OFFSET			9.93512f

#define MIDDLE_SEPARATOR_HEIGHT						0.2f
#define MIDDLE_SEPARATOR_TOP_Y_OFFSET				10.39160f

#define EDIT_BOX_WIDTH								1.71851f
#define EDIT_BOX_HEIGHT								0.59074f

#define PROPERTY_LABEL_WIDTH						0.64444f
#define PROPERTY_LABEL_HEIGHT						0.59074f
#define PROPERTY_VERTICAL_STEP						1.10092f
#define H_PROPERTY_LABEL_TOP_Y_OFFSET				11.25085f
#define TRANSPARENCY_LABEL_WIDTH					2.36295f

#define BUTTON_WIDTH								1.90647f
#define BUTTON_HEIGHT								0.51018f
#define CANCEL_BUTTON_X_OFFSET						4.02775f

#define BOTTOM_SEPARATOR_HEIGHT						0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET			0.64444f

#define DEFAULT_CURRENT_COLOR_H						83.0f
#define DEFAULT_CURRENT_COLOR_S						100.0f
#define DEFAULT_CURRENT_COLOR_V						73.0f
#define DEFAULT_CURRENT_COLOR_A						100.0f

#define HSV_FACTOR									0.016666f
#define HSV_TO_RGB_FACTOR							2.55f
#define ALPHA_FACTOR								2.55f

#define MAX_BUFFER_SYMBOLS							128


EMUIColorPicker* EMUIColorPicker::instance = nullptr;

EMUIColorPicker& EMUIColorPicker::GetInstance ()
{
	if ( !instance )
		instance = new EMUIColorPicker ();

	return *instance;
}

EMUIColorPicker::~EMUIColorPicker ()
{
	free ( buffer );

	delete pick;
	delete cancel;
	delete bottomSeparator;
	delete transparency;
	delete transparencyLabel;
	delete b;
	delete bLabel;
	delete v;
	delete vLabel;
	delete g;
	delete gLabel;
	delete s;
	delete sLabel;
	delete r;
	delete rLabel;
	delete h;
	delete hLabel;
	delete middleSeparator;

	for ( GXUByte i = 0; i < 16; i++ )
		delete savedColors[ i ];

	delete addColor;
	delete oldColor;
	delete currentColor;
	delete hsvColorWidget;
	delete topSeparator;
	delete caption;
	delete mainPanel;

	instance = nullptr;
}

GXWidget* EMUIColorPicker::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUIColorPicker::PickHSVColor ( GXVoid* handler, PFNEMONHSVCOLORPROC callback )
{
	this->handler = handler;
	OnHSVColor = callback;
	OnRGBColor = nullptr;
	mainPanel->Show ();
}

GXVoid EMUIColorPicker::PickRGBColor ( GXVoid* handler, PFNEMONRGBCOLORPROC callback )
{
	this->handler = handler;
	OnRGBColor = callback;
	OnHSVColor = nullptr;
	mainPanel->Hide ();
}

EMUIColorPicker::EMUIColorPicker () :
EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( nullptr );
	caption = new EMUIStaticText ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );

	GXWidget* mainPanelWidget = mainPanel->GetWidget ();
	hsvColorWidget = new GXUIInput ( mainPanelWidget, GX_TRUE );
	currentColor = new GXUIInput ( mainPanelWidget, GX_TRUE );
	oldColor = new GXUIInput ( mainPanelWidget, GX_TRUE );
	addColor = new EMUIButton ( mainPanel );

	for ( GXUByte i = 0; i < 16; i++ )
		savedColors[ i ] = new GXUIInput ( mainPanelWidget, GX_TRUE );

	middleSeparator = new EMUISeparator ( mainPanel );

	hLabel = new EMUIStaticText ( mainPanel );
	h = new EMUIEditBox ( mainPanel );

	rLabel = new EMUIStaticText ( mainPanel );
	r = new EMUIEditBox ( mainPanel );

	sLabel = new EMUIStaticText ( mainPanel );
	s = new EMUIEditBox ( mainPanel );

	gLabel = new EMUIStaticText ( mainPanel );
	g = new EMUIEditBox ( mainPanel );

	vLabel = new EMUIStaticText ( mainPanel );
	v = new EMUIEditBox ( mainPanel );

	bLabel = new EMUIStaticText ( mainPanel );
	b = new EMUIEditBox ( mainPanel );

	transparencyLabel = new EMUIStaticText ( mainPanel );
	transparency = new EMUIEditBox ( mainPanel );

	bottomSeparator = new EMUISeparator ( mainPanel );

	cancel = new EMUIButton ( mainPanel );
	pick = new EMUIButton ( mainPanel );

	GXLocale& locale = GXLocale::GetInstance ();

	hsvColorWidget->SetHandler ( this );
	hsvColorWidget->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );

	oldColor->SetHandler ( this );
	oldColor->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );

	for ( GXUByte i = 0; i < 16; i++ )
	{
		GXUIInput* input = savedColors[ i ];
		input->SetHandler ( this );
		input->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );
	}

	caption->SetText ( locale.GetString ( L"Color picker->Color picker" ) );
	caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
	caption->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );

	hLabel->SetText ( locale.GetString ( L"Color picker->H" ) );
	hLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	hLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	rLabel->SetText ( locale.GetString ( L"Color picker->R" ) );
	rLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	rLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	sLabel->SetText ( locale.GetString ( L"Color picker->S" ) );
	sLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	sLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	gLabel->SetText ( locale.GetString ( L"Color picker->G" ) );
	gLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	gLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	vLabel->SetText ( locale.GetString ( L"Color picker->V" ) );
	vLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	vLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	bLabel->SetText ( locale.GetString ( L"Color picker->B" ) );
	bLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	bLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	transparencyLabel->SetText ( locale.GetString ( L"Color picker->Transparency" ) );
	transparencyLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	transparencyLabel->SetAlingment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_LEFT );

	h->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	r->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	s->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	g->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	v->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	b->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );
	transparency->SetAlignment ( eGXUITextAlignment::GX_UI_TEXT_ALIGNMENT_CENTER );

	addColor->SetCaption ( L"+" );
	cancel->SetCaption ( locale.GetString ( L"Color picker->Cancel" ) );
	pick->SetCaption ( locale.GetString ( L"Color picker->Pick" ) );

	addColor->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );
	cancel->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );
	pick->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );

	mainPanel->SetOnResizeCallback ( this, &EMUIColorPicker::OnResize );

	buffer = (GXWChar*)malloc ( MAX_BUFFER_SYMBOLS * sizeof ( GXWChar ) );

	UpdateCurrentColor ( DEFAULT_CURRENT_COLOR_H, DEFAULT_CURRENT_COLOR_S, DEFAULT_CURRENT_COLOR_V, DEFAULT_CURRENT_COLOR_A );
	oldColorHSV = currentColorHSV;

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
	mainPanel->Hide ();
}

GXVoid EMUIColorPicker::UpdateCurrentColor ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha )
{
	currentColorHSV = GXCreateVec4 ( hue, saturation, value, alpha );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", hue );
	h->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", saturation );
	s->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", value );
	v->SetText ( buffer );

	GXUByte r;
	GXUByte g;
	GXUByte b;
	GXUByte a;
	GXConvertHSVToRGB ( currentColorHSV, r, g, b, a );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", r );
	this->r->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", g );
	this->g->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", b );
	this->b->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", a );
	transparency->SetText ( buffer );
}

GXVoid EMUIColorPicker::UpdateCurrentColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", red );
	this->r->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", green );
	this->g->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", blue );
	this->b->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", alpha );
	transparency->SetText ( buffer );

	GXVec4 rgbColor;
	GXColorToVec4 ( rgbColor, red, green, blue, alpha );

	GXFloat h;
	GXFloat s;
	GXFloat v;
	GXFloat a;
	GXConvertRGBToHSV ( rgbColor, h, s, v, a );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", h );
	this->h->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", s );
	this->s->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", v );
	this->v->SetText ( buffer );
}

GXVoid GXCALL EMUIColorPicker::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	//TODO
	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;
	colorPicker->mainPanel->Hide ();
}

GXVoid GXCALL EMUIColorPicker::OnLeftMouseButton ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y )
{
	//TODO
	GXLogW ( L"0x%016llX, %f, %f\n", (GXUPointer)input, x, y );
}

GXVoid GXCALL EMUIColorPicker::OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height )
{
	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;

	GXFloat margin = MARGIN * gx_ui_Scale;

	colorPicker->caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );
	colorPicker->topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );
	colorPicker->hsvColorWidget->Resize ( margin, height - ( MARGIN + HSV_COLOR_WIDGET_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, HSV_COLOR_WIDGET_HEIGHT * gx_ui_Scale );

	GXFloat colorSeparatorY = height - ( MARGIN + COLOR_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat colorSeperatorWidth = ( width - 2.0f * margin ) * 0.5f;
	GXFloat colorSeparatorHeight = COLOR_SEPARATOR_HEIGHT * gx_ui_Scale;
	colorPicker->currentColor->Resize ( margin, colorSeparatorY, colorSeperatorWidth, colorSeparatorHeight );
	colorPicker->oldColor->Resize ( margin + colorSeperatorWidth, colorSeparatorY, colorSeperatorWidth, colorSeparatorHeight );

	colorPicker->addColor->Resize ( margin, height - ( MARGIN + ADD_COLOR_BUTTON_TOP_Y_OFFSET ) * gx_ui_Scale, ADD_COLOR_BUTTON_WIDTH * gx_ui_Scale, ADD_COLOR_BUTTON_HEIGHT * gx_ui_Scale );

	GXFloat savedColorX = width - margin - SAVED_COLOR_RIGHT_OFFSET * gx_ui_Scale;
	GXFloat savedColorTopRowY = height - ( MARGIN + SAVED_COLOR_TOP_ROW_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat savedColorBottomRowY = height - ( MARGIN + SAVED_COLOR_BOTTOM_ROW_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat savedColorWidth = SAVED_COLOR_WIDTH * gx_ui_Scale;
	GXFloat savedColorHeight = SAVED_COLOR_HEIGHT * gx_ui_Scale;
	GXFloat savedColorHorizontalStep = SAVED_COLOR_HORIZONTAL_STEP * gx_ui_Scale;
	for ( GXUByte i = 0; i < 8; i++ )
	{
		colorPicker->savedColors[ i ]->Resize ( savedColorX, savedColorTopRowY, savedColorWidth, savedColorHeight );
		colorPicker->savedColors[ i + 8 ]->Resize ( savedColorX, savedColorBottomRowY, savedColorWidth, savedColorHeight );

		savedColorX += savedColorHorizontalStep;
	}

	colorPicker->middleSeparator->Resize ( margin, height - ( MARGIN + MIDDLE_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, MIDDLE_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat propertyY = height - margin - H_PROPERTY_LABEL_TOP_Y_OFFSET * gx_ui_Scale;
	GXFloat propertyVerticalStep = PROPERTY_VERTICAL_STEP * gx_ui_Scale;
	GXFloat propertyLabelWidth = PROPERTY_LABEL_WIDTH * gx_ui_Scale;
	GXFloat propertyLabelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
	GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
	GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
	GXFloat leftColumnEditBoxX = margin + propertyLabelWidth;
	GXFloat rightColumnPropertyLabelX = width - margin - editBoxWidth - propertyLabelWidth;
	GXFloat rightColumnEditBoxX = width - margin - editBoxWidth;

	colorPicker->hLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->h->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->rLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->r->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	propertyY -= propertyVerticalStep;

	colorPicker->sLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->s->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->gLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->g->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	propertyY -= propertyVerticalStep;

	colorPicker->vLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->v->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->bLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->b->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	propertyY -= propertyVerticalStep;

	colorPicker->transparencyLabel->Resize ( margin, propertyY, TRANSPARENCY_LABEL_WIDTH * gx_ui_Scale, propertyLabelHeight );
	colorPicker->transparency->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
	GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

	colorPicker->cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
	colorPicker->pick->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}
