//version 1.2


#include <GXEngine/GXGlobals.h>
#include <Editor Mobile/EMMain.h>
//#include <GXCommon/GXMTRSaver.h>
//#include <GXCommon/GXBKESaver.h>
//#include <Test/GXGameTest.h>
//#include <Thesis/TSGameMain.h>
//#include <MLP Ultimate/GXGameMLPUltimate.h>


int wmain ( int argc, LPWSTR commandline )
{
	//#define GX_CREATE_MTR_FILE
	#ifdef GX_CREATE_MTR_FILE
		GXMaterialInfo matInfo;
	
		matInfo.numTextures = 5;
		GXTextureInfo textures[ 17 ];

		//SetCurrentDirectoryW ( L".." );
	
		textures[ 0 ].fileName = L"../Textures/Test/Fluttershy_Diffuse.tga";
		textures[ 0 ].cacheFileName = L"../Textures/Test/Cache/Fluttershy_Diffuse.cache";
	
		textures[ 1 ].fileName = L"../Textures/System/NOT_Emit.tga";
		textures[ 1 ].cacheFileName = L"../Textures/System/Cache/NOT_Emit.cache";

		textures[ 2 ].fileName = L"../Textures/Museum/Default_Specular.tga";
		textures[ 2 ].cacheFileName = L"../Textures/Museum/Cache/Default_Specular.cache";

		textures[ 3 ].fileName = L"../Textures/Test/Fluttershy_Diffuse.tga";
		textures[ 3 ].cacheFileName = L"../Textures/Test/Cache/Fluttershy_Diffuse.cache";

		textures[ 4 ].fileName = L"../Textures/Thesis/Brick_Wall_Normals.tga";
		textures[ 4 ].cacheFileName = L"../Textures/Thesis/Cache/Brick_Wall_Normals.cache";
		
		textures[ 5 ].fileName = L"../Textures/Thesis/Brick_Wall_Normals.tga";
		textures[ 5 ].cacheFileName = L"../Textures/Thesis/Cache/Brick_Wall_Normals Escape.cache";

		textures[ 6 ].fileName = L"../Textures/Thesis/XBox Down.tga";
		textures[ 6 ].cacheFileName = L"../Textures/Thesis/Cache/XBox Down.cache";

		textures[ 7 ].fileName = L"../Textures/Thesis/XBox Up.tga";
		textures[ 7 ].cacheFileName = L"../Textures/Thesis/Cache/XBox Up.cache";

		textures[ 8 ].fileName = L"../Textures/Thesis/XBox Right.tga";
		textures[ 8 ].cacheFileName = L"../Textures/Thesis/Cache/XBox Right.cache";

		textures[ 9 ].fileName = L"../Textures/Thesis/XBox Left.tga";
		textures[ 9 ].cacheFileName = L"../Textures/Thesis/Cache/XBox Left.cache";

		textures[ 10 ].fileName = L"../Textures/Thesis/XBox A.tga";
		textures[ 10 ].cacheFileName = L"../Textures/Thesis/Cache/XBox A.cache";

		textures[ 11 ].fileName = L"../Textures/Thesis/XBox B.tga";
		textures[ 11 ].cacheFileName = L"../Textures/Thesis/Cache/XBox B.cache";

		textures[ 12 ].fileName = L"../Textures/Thesis/XBox X.tga";
		textures[ 12 ].cacheFileName = L"../Textures/Thesis/Cache/XBox X.cache";

		textures[ 13 ].fileName = L"../Textures/Thesis/XBox Y.tga";
		textures[ 13 ].cacheFileName = L"../Textures/Thesis/Cache/XBox Y.cache";

		textures[ 14 ].fileName = L"../Textures/Thesis/Menu_Background.tga";
		textures[ 14 ].cacheFileName = L"../Textures/Thesis/Cache/Menu_Background.cache";

		textures[ 15 ].fileName = L"../Textures/Thesis/Selector.tga";
		textures[ 15 ].cacheFileName = L"../Textures/Thesis/Cache/Selector.cache";

		textures[ 16 ].fileName = L"../Textures/Thesis/Keyboard.tga";
		textures[ 16 ].cacheFileName = L"../Textures/Thesis/Cache/Keyboard.cache";
	
		matInfo.textures = textures;
	
		matInfo.numShaders = 1;
		GXShaderInfo shaders[ 8 ];

		shaders[ 0 ].vShaderFileName = L"../Shaders/System/NativeSkin_vs.txt";
		shaders[ 0 ].gShaderFileName = 0;
		shaders[ 0 ].fShaderFileName = L"../Shaders/Thesis/NativeSkin_fs.txt";

		shaders[ 1 ].vShaderFileName = L"Shaders/Museum/Mesh_vs.txt";
		shaders[ 1 ].gShaderFileName = 0;
		shaders[ 1 ].fShaderFileName = L"Shaders/Museum/Bulp_fs.txt";

		shaders[ 2 ].vShaderFileName = L"Shaders/Museum/Mesh_vs.txt";
		shaders[ 2 ].gShaderFileName = 0;
		shaders[ 2 ].fShaderFileName = L"Shaders/Museum/Spot_fs.txt";
	
		shaders[ 3 ].vShaderFileName = L"Shaders/Museum/Mesh_vs.txt";
		shaders[ 3 ].gShaderFileName = 0;
		shaders[ 3 ].fShaderFileName = L"Shaders/Museum/Directed_fs.txt";

		shaders[ 4 ].vShaderFileName = L"../Shaders/Thesis/VertexOnly_vs.txt";
		shaders[ 4 ].gShaderFileName = 0;
		shaders[ 4 ].fShaderFileName = L"../Shaders/Thesis/SpotLampShadows_fs.txt";
	
		shaders[ 5 ].vShaderFileName = L"../Shaders/Thesis/VertexOnly_vs.txt";
		shaders[ 5 ].gShaderFileName = 0;
		shaders[ 5 ].fShaderFileName = L"../Shaders/Thesis/SpotLamp_fs.txt";

		shaders[ 6 ].vShaderFileName = L"../Shaders/Thesis/VertexOnly_vs.txt";
		shaders[ 6 ].gShaderFileName = 0;
		shaders[ 6 ].fShaderFileName = L"../Shaders/Thesis/Null_fs.txt";

		matInfo.shaders = shaders;
	
		GXExportMTR ( L"../Materials/Thesis/Fluttershy.mtr", matInfo );
		return 0;
		#undef GX_CREATE_MTR_FILE
	#endif

	//#define GX_EXTEND_MTR_FILE
	#ifdef GX_EXTEND_MTR_FILE
		GXMaterialInfo	matInfo;
		GXLoadMTR ( L"../Materials/Thesis/GBuffer.mtr", matInfo );

		GXMaterialInfo	newMatInfo;
		newMatInfo.numTextures = matInfo.numTextures;
		newMatInfo.numShaders = matInfo.numShaders + 1;

		newMatInfo.textures = (GXTextureInfo*)malloc ( newMatInfo.numShaders * sizeof ( GXTextureInfo ) );
		for ( GXUInt i = 0; i < matInfo.numTextures; i++ )
		{
			newMatInfo.textures[ i ].fileName = matInfo.textures[ i ].fileName;
			newMatInfo.textures[ i ].cacheFileName = matInfo.textures[ i ].cacheFileName;
		}

		newMatInfo.shaders = (GXShaderInfo*)malloc ( newMatInfo.numShaders * sizeof ( GXShaderInfo ) );

		for ( GXUInt i = 0; i < matInfo.numShaders; i++ )
		{
			newMatInfo.shaders[ i ].vShaderFileName = matInfo.shaders[ i ].vShaderFileName;
			newMatInfo.shaders[ i ].gShaderFileName = matInfo.shaders[ i ].gShaderFileName;
			newMatInfo.shaders[ i ].fShaderFileName = matInfo.shaders[ i ].fShaderFileName;
		}

		//Add new shader entry
		newMatInfo.shaders[ matInfo.numShaders ].vShaderFileName = L"../Shaders/Thesis/ScreenQuad_vs.txt";
		newMatInfo.shaders[ matInfo.numShaders ].gShaderFileName = 0;
		newMatInfo.shaders[ matInfo.numShaders ].fShaderFileName = L"../Shaders/Thesis/DirectedLightShadows_fs.txt";

		GXExportMTR ( L"../Materials/Thesis/GBuffer_Ext.mtr", newMatInfo );

		free ( newMatInfo.shaders );
		free ( newMatInfo.textures );

		matInfo.Cleanup ();

		return 0;
	#endif

	//#define GX_CREATE_BKE_FILE
	#ifdef GX_CREATE_BKE_FILE
		GXBakeInfo info;
		info.fileName = L"../Physics/Test/Triangle Meshes/Geometry/Ground.geo";
		info.cacheFileName = L"../Physics/Test/Triangle Meshes/Bake/Ground.bake";
		GXExportBKE ( L"../Physics/Test/Triangle Meshes/Ground.bke", info );
		return 0;
		#undef GX_CREATE_BKE_FILE
	#endif


	//#define GX_CREATE_CFG_FILE
	#ifdef GX_CREATE_CFG_FILE
		const GXWChar currentDir[] = L"/mnt/sdcard/GXEngine";
		GXUInt size = sizeof ( GXWChar ) * ( wcslen ( currentDir ) + 1 );
		GXWriteToFile ( L"D:/GXConfig.cfg", GX_TRUE, (GXVoid*)currentDir, size );
		return 0;
	#endif

	//gx_Core = new GXCore ( OnMLPUltimateGameInit, OnMLPUltimateGameClose, L"MLP - Ultimate" );
	//gx_Core = new GXCore ( OnLevelInit, OnLevelClose, L"GXEngine - Test game" );
	//gx_Core = new GXCore ( TSGameInit, TSGameClose, L"Thesis" );
	gx_Core = new GXCore ( &EMOnInit, &EMOnClose, L"GXEngine Editor Mobile" );
	
	gx_Core->Start ();

	delete gx_Core;

	return 0;
}

//----------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE hinst, HINSTANCE hprev, LPWSTR cmdLine, int mode )
{
	//gx_Core = new GXCore ( OnMLPUltimateGameInit, OnMLPUltimateGameClose, L"MLP - Ultimate" );
	//gx_Core = new GXCore ( OnLevelInit, OnLevelClose, L"GXEngine - Test game" );
	//gx_Core = new GXCore ( TSGameInit, TSGameClose, L"Thesis" );
	gx_Core = new GXCore ( &EMOnInit, &EMOnClose, L"GXEngine Editor Mobile" );

	gx_Core->Start ();
	delete gx_Core;

	return 0;
}