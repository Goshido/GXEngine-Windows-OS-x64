#include <GXEngine_Editor_Mobile/EMUIMenu.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXFontStorage.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


class EMUIMenuRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXTexture			background;
		GXFont*				font;
		GXDynamicArray		captions;
		GXFloat				layer;

		EMUIMenu*			menu;

		GLuint				maskSampler;
		GLint				mod_view_proj_matLocation;
		GXShaderProgram		maskProgram;

	public:
		EMUIMenuRenderer ( GXUIMenu* widget, EMUIMenu* menu );
		~EMUIMenuRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

		GXVoid OnDrawMask ();

		GXVoid AddItem ( const GXWChar* caption );
		GXVoid RenameItem ( GXUByte item, const GXWChar* caption );

		GXFloat EstimateItemWidth ( const GXWChar* caption );
		GXUByte GetTotalItems ();

		GXVoid SetLayer ( GXFloat z );
		GXFloat GetLayer ();

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIMenuRenderer::EMUIMenuRenderer ( GXUIMenu* widget, EMUIMenu* menu ):
GXWidgetRenderer ( widget ), captions ( sizeof ( GXWChar* ) )
{
	this->menu = menu;

	surface = new GXHudSurface ( (GXUShort)( gx_ui_Scale * 4.0f ), (GXUShort)( gx_ui_Scale * 0.5f ), GX_FALSE );
	font = GXGetFont ( L"Fonts/trebuc.ttf", (GXUShort)( gx_ui_Scale * 0.33f ) );
	GXLoadTexture ( L"Textures/System/Default_Diffuse.tga", background );
	SetLayer ( EMGetNextGUIForegroundZ () );

	GXGLSamplerStruct ss;
	ss.anisotropy = 16.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_NONE;
	ss.wrap = GL_CLAMP_TO_EDGE;
	maskSampler = GXCreateSampler ( ss );

	GXGetShaderProgram ( maskProgram, L"Shaders/Editor Mobile/MaskVertexAndUV_vs.txt", 0, L"Shaders/Editor Mobile/MaskAlphaTest_fs.txt" );

	if ( !maskProgram.isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "alphaSampler" };

		GXTuneShaderSamplers ( maskProgram, samplerIndexes, samplerNames, 1 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( maskProgram.program, "mod_view_proj_mat" );
}

EMUIMenuRenderer::~EMUIMenuRenderer ()
{
	GXRemoveTexture ( background );
	GXRemoveFont ( font );
	delete surface;

	GXUByte len = (GXUByte)captions.GetLength ();
	for ( GXUByte i = 0; i < len; i++ )
	{
		GXWChar* caption = *( (GXWChar**)captions.GetValue ( i ) );
		GXSafeFree ( caption );
	}

	GXRemoveShaderProgram ( maskProgram );

	glDeleteSamplers ( 1, &maskSampler );
}

GXVoid EMUIMenuRenderer::OnRefresh ()
{
	GXImageInfo ii;
	GXPenInfo pi;

	ii.texture = background;
	GXColorToVec4 ( ii.color, 49, 48, 48, 255 );
	ii.insertX = ii.insertY = 0.5f;
	ii.insertWidth = GXGetAABBWidth ( widget->GetBoundsWorld () );
	ii.insertHeight = GXGetAABBHeight ( widget->GetBoundsWorld () );
	ii.overlayType = GX_SIMPLE_REPLACE;

	surface->AddImage ( ii );

	pi.font = font;
	GXColorToVec4 ( pi.color, 115, 185, 0 , 255 );
	pi.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;

	GXUIMenu* menu = (GXUIMenu*)widget;

	GXUByte total = menu->GetTotalItems ();
	for ( GXUByte i = 0; i < total; i++ )
	{
		const GXAABB& itemBounds = menu->GetItemBounds ( i );
		ii.insertX = itemBounds.min.x;
		ii.insertY = 0;
		ii.insertWidth = itemBounds.max.x - itemBounds.min.x;
		ii.insertHeight = itemBounds.max.y - itemBounds.min.y;

		if ( menu->IsItemHighlighted ( i ) )
			GXColorToVec4 ( ii.color, 46, 64, 11, 255 );
		else
			GXColorToVec4 ( ii.color, 49, 48, 48, 255 );

		surface->AddImage ( ii );

		GXWChar* caption = *( (GXWChar**)captions.GetValue ( i ) );

		pi.insertX = ii.insertX + 0.25f * gx_ui_Scale;
		pi.insertY = ( ii.insertHeight - font->GetSize () * 0.8f ) * 0.5f;
		
		surface->AddText ( pi, 0, caption );
	}
}

GXVoid EMUIMenuRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIMenuRenderer::OnDrawMask ()
{
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, surface->GetModelMatrix (), GXCamera::GetActiveCamera ()->GetViewMatrix () );

	const GXVAOInfo& surfaceVAOInfo = surface->GetMeshVAOInfo ();
	
	glUseProgram ( maskProgram.program );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, surface->GetTexture () );
	glBindSampler ( 0, maskSampler );

	glBindVertexArray ( surfaceVAOInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, surfaceVAOInfo.numVertices );
	glBindVertexArray ( 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 0, 0 );

	glUseProgram ( 0 );
}

GXVoid EMUIMenuRenderer::AddItem ( const GXWChar* caption )
{
	GXWChar* str;
	GXWcsclone ( &str, caption );

	GXUByte total = (GXUByte)captions.GetLength ();
	captions.SetValue ( total, &str );
}

GXVoid EMUIMenuRenderer::RenameItem ( GXUByte item, const GXWChar* caption )
{
	GXWChar* str = *( (GXWChar**)captions.GetValue ( item ) );
	GXSafeFree ( str );
	GXWcsclone ( &str, caption );
	captions.SetValue ( item, &str );
}

GXFloat EMUIMenuRenderer::EstimateItemWidth ( const GXWChar* caption )
{
	GXUInt len = font->GetTextLength ( 0, caption );
	return len + 0.5f * gx_ui_Scale;
}

GXUByte EMUIMenuRenderer::GetTotalItems ()
{
	return (GXUByte)captions.GetLength ();
}

GXVoid EMUIMenuRenderer::SetLayer ( GXFloat z )
{
	layer = z;
	GXVec3 location;
	surface->GetLocation ( location );
	location.z = z;
	surface->SetLocation ( location );

	menu->OnChangeLayer ( z );
}

GXFloat EMUIMenuRenderer::GetLayer ()
{
	return layer;
}

GXVoid EMUIMenuRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	delete surface;
	surface = new GXHudSurface ( width, height, GX_FALSE );
	surface->SetLocation ( x, y, layer );
}

GXVoid EMUIMenuRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	surface->SetLocation ( x, y, layer );
}

//------------------------------------------------------------------------------------------

struct EMUISubmenuItem
{
	GXWChar*	caption;
	GXWChar*	hotkey;
};

class EMUISubmenuRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXTexture			background;
		GXFont*				font;
		GXDynamicArray		items;
		GXFloat				layer;

		GLuint				maskSampler;
		GLint				mod_view_proj_matLocation;
		GXShaderProgram		maskProgram;

	public:
		EMUISubmenuRenderer ( GXUISubmenu* widget );
		~EMUISubmenuRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

		GXVoid OnDrawMask ();

		GXVoid AddItem ( const GXWChar* caption, const GXWChar* hotkey );
		GXVoid RenameItem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey );

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUISubmenuRenderer::EMUISubmenuRenderer ( GXUISubmenu* widget ):
GXWidgetRenderer ( widget ), items ( sizeof ( EMUISubmenuItem ) )
{
	surface = new GXHudSurface ( (GXUShort)( gx_ui_Scale * 8.0f ), (GXUShort)( gx_ui_Scale * 2.0f ), GX_FALSE );
	font = GXGetFont ( L"Fonts/trebuc.ttf", (GXUShort)( gx_ui_Scale * 0.33f ) );
	GXLoadTexture ( L"Textures/System/Default_Diffuse.tga", background );
	layer = 1.0f;

	GXGLSamplerStruct ss;
	ss.anisotropy = 16.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_NONE;
	ss.wrap = GL_CLAMP_TO_EDGE;
	maskSampler = GXCreateSampler ( ss );

	GXGetShaderProgram ( maskProgram, L"Shaders/Editor Mobile/MaskVertexAndUV_vs.txt", 0, L"Shaders/Editor Mobile/MaskAlphaTest_fs.txt" );

	if ( !maskProgram.isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "alphaSampler" };

		GXTuneShaderSamplers ( maskProgram, samplerIndexes, samplerNames, 1 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( maskProgram.program, "mod_view_proj_mat" );
}

EMUISubmenuRenderer::~EMUISubmenuRenderer ()
{
	GXRemoveTexture ( background );
	GXRemoveFont ( font );
	delete surface;

	GXUByte len = (GXUByte)items.GetLength ();
	for ( GXUByte i = 0; i < len; i++ )
	{
		EMUISubmenuItem* item = (EMUISubmenuItem*)items.GetValue ( i );
		GXSafeFree ( item->caption );
		GXSafeFree ( item->hotkey );
	}

	GXRemoveShaderProgram ( maskProgram );

	glDeleteSamplers ( 1, &maskSampler );
}

GXVoid EMUISubmenuRenderer::OnRefresh ()
{
	const GXAABB& bounds = widget->GetBoundsWorld ();

	surface->Reset ();
	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	GXImageInfo ii;
	ii.texture = background;
	GXColorToVec4 ( ii.color, 49, 48, 48, 255 );
	ii.insertX = ii.insertY = 0.5f;
	ii.insertWidth = w;
	ii.insertHeight = h;
	ii.overlayType = GX_SIMPLE_REPLACE;
	surface->AddImage ( ii );

	GXUISubmenu* submenu = (GXUISubmenu*)widget;
	GXUByte total = submenu->GetTotalItems ();
	GXFloat itemHeight = submenu->GetItemHeight ();

	GXPenInfo pi;
	pi.font = font;
	pi.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;

	for ( GXUByte i = 0; i < total; i++ )
	{
		if ( submenu->IsItemPressed ( i ) )
		{
			GXColorToVec4 ( ii.color, 83, 116, 20, 255 );
			GXColorToVec4 ( pi.color, 142, 255, 5, 255 );
			ii.insertY = ( total - i - 1 ) * itemHeight;
			ii.insertHeight = itemHeight;
			surface->AddImage ( ii );
		}
		else if ( submenu->IsItemHighlighted ( i ) )
		{
			GXColorToVec4 ( ii.color, 46, 64, 11, 255 );
			GXColorToVec4 ( pi.color, 115, 185, 0, 255 );
			ii.insertY = ( total - i - 1 ) * itemHeight;
			ii.insertHeight = itemHeight;
			surface->AddImage ( ii );
		}
		else
			GXColorToVec4 ( pi.color, 115, 185, 0, 255 );

		const EMUISubmenuItem* item = (const EMUISubmenuItem*)items.GetValue ( i );

		pi.insertX = 0.15f * gx_ui_Scale;
		pi.insertY = 0.15f * gx_ui_Scale + ( total - i - 1 ) * itemHeight;
		
		surface->AddText ( pi, 0, item->caption );

		if ( !item->hotkey ) continue;

		GXInt len = (GXInt)font->GetTextLength ( 0, item->hotkey );
		pi.insertX = ii.insertWidth - len - 0.15f * gx_ui_Scale;
		surface->AddText ( pi, 0, item->hotkey );
	}

	GXLineInfo li;
	GXColorToVec4 ( li.color, 128, 128, 128, 255 );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec3 ( 1.5f, 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( w - 0.5f, 0.5f, 0.0f );
	li.overlayType = GX_SIMPLE_REPLACE;

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( w - 0.5f, 1.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( w - 0.5f, h - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( w - 1.5f, h - 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, h - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( 0.5f, h - 1.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, 0.5f, 0.0f );

	surface->AddLine ( li );
}

GXVoid EMUISubmenuRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUISubmenuRenderer::OnDrawMask ()
{
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, surface->GetModelMatrix (), GXCamera::GetActiveCamera ()->GetViewMatrix () );

	const GXVAOInfo& surfaceVAOInfo = surface->GetMeshVAOInfo ();
	
	glUseProgram ( maskProgram.program );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, surface->GetTexture () );
	glBindSampler ( 0, maskSampler );

	glBindVertexArray ( surfaceVAOInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, surfaceVAOInfo.numVertices );
	glBindVertexArray ( 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 0, 0 );

	glUseProgram ( 0 );
}

GXVoid EMUISubmenuRenderer::AddItem ( const GXWChar* caption, const GXWChar* hotkey )
{
	EMUISubmenuItem smi;
	GXWcsclone ( &smi.caption, caption );
	if ( hotkey )
		GXWcsclone ( &smi.hotkey, hotkey );
	else
		smi.hotkey = 0;

	items.SetValue ( items.GetLength (), &smi ); 
}

GXVoid EMUISubmenuRenderer::RenameItem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey )
{
	EMUISubmenuItem* p = (EMUISubmenuItem*)items.GetValue ( item );
	GXSafeFree ( p->caption );
	GXSafeFree ( p->hotkey );
	
	GXWcsclone ( &p->caption, caption );
	if ( hotkey )
		GXWcsclone ( &p->hotkey, hotkey );
	else
		p->hotkey = nullptr;
}

GXVoid EMUISubmenuRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	delete surface;
	surface = new GXHudSurface ( width, height, GX_FALSE );
	surface->SetLocation ( x, y, layer );
}

GXVoid EMUISubmenuRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	surface->SetLocation ( x, y, layer );
}

//------------------------------------------------------------------------------------------

EMUIMenu::EMUIMenu ():
EMUI ( nullptr ), submenuWidgets ( sizeof ( GXUISubmenu* ) )
{
	menuWidget = new GXUIMenu ( nullptr, this, &OnShowSubmenu );
	menuWidget->SetRenderer ( new EMUIMenuRenderer ( menuWidget, this ) );
	menuWidget->Resize ( 0.0f, 0.0f, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
}

EMUIMenu::~EMUIMenu ()
{
	delete menuWidget->GetRenderer ();
	delete menuWidget;

	GXUInt total = submenuWidgets.GetLength ();
	for ( GXUInt i = 0; i < total; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( i ) );
		delete submenu->GetRenderer ();
		delete submenu;
	}
}

GXVoid EMUIMenu::OnDrawMask ()
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();
	renderer->OnDrawMask ();

	GXUByte total = (GXUByte)submenuWidgets.GetLength ();
	for ( GXUByte i = 0; i < total; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( i ) );
		if ( submenu->IsVisible () )
		{
			EMUISubmenuRenderer* renderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
			renderer->OnDrawMask ();
		}
	}
}

GXVoid EMUIMenu::AddItem ( const GXWChar* caption )
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();

	GXUByte total = renderer->GetTotalItems ();
	renderer->AddItem ( caption );

	menuWidget->AddItem ();
	GXFloat r = renderer->EstimateItemWidth ( caption );
	menuWidget->ResizeItem ( total, r );

	GXUISubmenu* submenu = new GXUISubmenu ( nullptr );
	submenu->SetRenderer ( new EMUISubmenuRenderer ( submenu ) );
	submenu->Hide ();
	submenu->Resize ( 0.0f, 0.0f, 8.0f * gx_ui_Scale, 0.5f * gx_ui_Scale );

	submenuWidgets.SetValue ( total, &submenu );
}

GXVoid EMUIMenu::RenameItem ( GXUByte item, const GXWChar* caption )
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();
	menuWidget->ResizeItem ( item, renderer->EstimateItemWidth ( caption ) );
	renderer->RenameItem ( item, caption );
}

GXVoid EMUIMenu::SetLocation ( GXFloat x, GXFloat y )
{
	const GXAABB& menuBounds = menuWidget->GetBoundsWorld ();
	menuWidget->Resize ( x, y, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
}

GXVoid EMUIMenu::AddSubitem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey, PFNGXONMENUITEMPROC onMouseButton )
{
	GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( item ) );
	EMUIMenuRenderer* menuRenderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();
	EMUISubmenuRenderer* renderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
	submenu->AddItem ( onMouseButton );
	renderer->AddItem ( caption, hotkey );
}

GXVoid EMUIMenu::RenameSubitem ( GXUByte item, GXUByte subitem, const GXWChar* caption, const GXWChar* hotkey )
{
	GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( item ) );
	EMUISubmenuRenderer* renderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
	renderer->RenameItem ( item, caption, hotkey );
	submenu->Redraw ();
}

GXVoid EMUIMenu::SetLayer ( GXFloat z )
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();
	renderer->SetLayer ( z );
}

GXVoid EMUIMenu::OnChangeLayer ( GXFloat z )
{
	GXUInt total = submenuWidgets.GetLength ();
	for ( GXUInt i = 0; i < total; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( i ) );
		EMUISubmenuRenderer* submenuRenderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
	}
}

GXVoid GXCALL EMUIMenu::OnShowSubmenu ( GXVoid* menuHandler, GXUByte item )
{
	EMUIMenu* menu = (EMUIMenu*)menuHandler;

	GXUByte totalSubmenus = (GXUByte)menu->submenuWidgets.GetLength ();
	for ( GXUByte i = 0; i < totalSubmenus; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)menu->submenuWidgets.GetValue ( i ) );
		if ( i != item && submenu->IsVisible () )
			submenu->Hide ();
	}

	GXUISubmenu* activeSubmenu = *( (GXUISubmenu**)menu->submenuWidgets.GetValue ( item ) );
	const GXAABB& itemBounds = menu->menuWidget->GetItemBounds ( item );

	activeSubmenu->Resize ( itemBounds.min.x, itemBounds.min.y - ( activeSubmenu->GetTotalItems () * activeSubmenu->GetItemHeight () ), gx_ui_Scale, gx_ui_Scale );
	activeSubmenu->Show ();

	EMUIMenuRenderer* menuRenderer = (EMUIMenuRenderer* )menu->menuWidget->GetRenderer ();
	EMUISubmenuRenderer* submenuRenderer = (EMUISubmenuRenderer*)activeSubmenu->GetRenderer ();
}
