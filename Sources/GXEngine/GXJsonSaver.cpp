//version 1.1

#include <GXEngine/GXJsonSaver.h>
#include <GXCommon/GXOBJLoader.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>


GXVoid GXCALL GXExportJSON ( GXWChar* src_path_to_OBJ )
{
	GXOBJPoint* data;
	GXOBJPoint* p;

	GXInt totalpoints = GXLoadOBJ ( src_path_to_OBJ, &data );
	GXWChar* path_to_JSON;
	GXUShort len = wcslen ( src_path_to_OBJ );
	path_to_JSON = ( GXWChar* )malloc ( ( len + 6 ) * sizeof ( GXWChar ) );
	memcpy ( path_to_JSON, src_path_to_OBJ, len * sizeof ( GXWChar ) );

	GXWChar ext [] = L".json\0";
	memcpy ( path_to_JSON + len, ext, 6 * sizeof ( GXWChar ) );

	FILE* output;
	_wfopen_s ( &output, path_to_JSON, L"wt\0" );
	free ( path_to_JSON );
	setlocale ( LC_ALL, "C" ); 
	
	fprintf ( output, "{\n	\"materials\" :\n	[\n		{\n			\"vertexshader\" : \"../Shaders/vs-mech.txt\",\n			\"fragmentshader\" : \"../Shaders/fs-mech.txt\",\n			\"numindices\" : %i,\n			\"diffuse\" : \"../Textures/mech-diffuse.jpg\",\n 			\"emissive\" : \"../Textures/mech-emissive.jpg\"\n		}\n	],\n	\"vertexPositions\" : [", totalpoints );
	
	//Здесь записываем 3d-координаты
	GXInt offset = 0;
	p = data;

	fprintf ( output, "%f, %f, %f", p[ offset ].vertex.x, p[ offset ].vertex.y, p[ offset ].vertex.z );
	offset++;

	while ( offset < totalpoints )
	{
		fprintf ( output, ", %f, %f, %f", p[ offset ].vertex.x, p[ offset ].vertex.y, p[ offset ].vertex.z );
		offset++;
	}
	
	fprintf ( output, "],\n	\"vertexNormals\" : [" );
	
	//Здесь записываем компоненты нормалей
	offset = 0;
	p = data;

	fprintf ( output, "%f, %f, %f", p[ offset ].normal.x, p[ offset ].normal.y, p[ offset ].normal.z );
	offset++;

	while ( offset < totalpoints )
	{
		fprintf ( output, ", %f, %f, %f", p[ offset ].normal.x, p[ offset ].normal.y, p[ offset ].normal.z );
		offset++;
	}

	fprintf ( output, "],\n	\"vertexTextureCoords\" : [" );
	
	//Здесь записываем текстурные координаты
	offset = 0;
	p = data;

	fprintf ( output, "%f, %f", p[ offset ].uv.u, p[ offset ].uv.v );
	offset++;


	while ( offset < totalpoints )
	{
		fprintf ( output, ", %f, %f", p[ offset ].uv.u, p[ offset ].uv.v );
		offset++;
	}

	fprintf ( output, "]\n}" );
	
	fclose ( output );
	free ( data );
}