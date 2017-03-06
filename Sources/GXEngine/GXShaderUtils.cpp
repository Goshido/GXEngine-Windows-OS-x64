//version 1.6

#include <GXEngine/GXShaderUtils.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


GLint GXCALL GXShaderCompilationStatus ( GLuint shader, const GXWChar* fileName )
{
	GLint status;

	glGetShaderiv ( shader, GL_COMPILE_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GXInt size = 0;
		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &size );

		GLchar* log = (GLchar*)malloc ( size );
		glGetShaderInfoLog ( shader, size, &size, log );

		GXLogW ( L"GXShaderCompilationStatus::Error - Шейдер %s\n", fileName );
		GXLogA ( "%s\n", log );

		free ( log );
	}

	#ifdef DEBUG_OPENGL_ERROR_TO_CONSOLE
		OPENGL_CHECK_FOR_ERRORS ();
	#endif

	return status;
}

GLint GXCALL GXShaderProgramStatus ( GLuint program, GLenum param, const GXWChar* vFileName, const GXWChar* gFileName, const GXWChar* fFileName )
{
	GLint status = 0;

	glGetProgramiv ( program, param, &status );

	if ( status != GL_TRUE )
	{
		GXInt size = 0;
		glGetShaderiv ( program, GL_INFO_LOG_LENGTH, &size );

		GLchar* log = (GLchar*)malloc ( size );
		glGetProgramInfoLog ( program, size, &size, log );

		switch ( param )
		{
			case GL_LINK_STATUS:
				GXLogW ( L"GXShaderProgramStatus::Error - Не могу слинковать шейдер %s + %s + %s\n", vFileName, gFileName, fFileName );
			break;

			case GL_VALIDATE_STATUS:
				GXLogW ( L"GXShaderProgramStatus::Error - Шейдер %s + %s + %s не прошёл валидацию\n", vFileName, gFileName, fFileName );
			break;
		}

		GXLogA ( "%s\n", log );

		free ( log );
	}

	#ifdef DEBUG_OPENGL_ERROR_TO_CONSOLE
		OPENGL_CHECK_FOR_ERRORS ();
	#endif

	return status;
}

GLuint GXCALL GXCreateShaderProgram ( const GXWChar* vFileName, const GXWChar* gFileName, const GXWChar* fFileName )
{
	GLuint shaderProgram = glCreateProgram ();
	GLuint vertexShader = glCreateShader ( GL_VERTEX_SHADER );

	GXChar* shaderSource;
	GXUInt shaderSourceLength;

	if ( !GXLoadFile ( vFileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
	{
		GXLogW ( L"GXCreateShaderProgram::Error - Не могу загрузить вершинный шейдер %s\n", vFileName );
		GXDebugBox ( L"GXCreateShaderProgram::Error - Вершинный шейдер не загрузился" );
	}

	glShaderSource ( vertexShader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
	glCompileShader ( vertexShader );
	GXSafeFree ( shaderSource );

	if ( GXShaderCompilationStatus ( vertexShader, vFileName ) != GL_TRUE )
	{
		GXLogW ( L"GXCreateShaderProgram::Error - Не могу скомпилировать вершинный шейдер %s\n", vFileName );
		GXDebugBox ( L"CreateShaderProgram::Error - Вершинный шейдер не скомпилировался" );
	}

	GLuint geometryShader = 0;
	if ( gFileName )
	{
		if ( !GXLoadFile ( gFileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
		{
			GXLogW ( L"GXCreateShaderProgram::Error - Не могу загрузить геометрический шейдер %s\n", gFileName );
			GXDebugBox ( L"GXCreateShaderProgram::Error - Геометрический шейдер не загрузился" );
		}

		geometryShader = glCreateShader ( GL_GEOMETRY_SHADER );

		glShaderSource ( geometryShader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
		glCompileShader ( geometryShader );
		GXSafeFree ( shaderSource );

		if ( GXShaderCompilationStatus ( geometryShader, gFileName ) != GL_TRUE )
		{
			GXLogW ( L"GXCreateShaderProgram::Error - Не могу скомпилировать геометрический шейдер %s\n", gFileName );
			GXDebugBox ( L"CreateShaderProgram::Error - Геометрический шейдер не скомпилировался" );
		}
	}

	if ( !GXLoadFile ( fFileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
	{
		GXLogW ( L"GXCreateShaderProgram::Error - Не могу загрузить фрагментный шейдер %s\n", fFileName );
		GXDebugBox ( L"CreateShaderProgram::Error - Фрагментный шейдер не загрузился" );
	}

	GLuint fragmentShader = glCreateShader ( GL_FRAGMENT_SHADER );
	glShaderSource ( fragmentShader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
	glCompileShader ( fragmentShader );
	GXSafeFree ( shaderSource );

	if ( GXShaderCompilationStatus ( fragmentShader, fFileName ) != GL_TRUE )
	{
		GXLogW ( L"GXCreateShaderProgram::Error - Не могу скомпилировать фрагментный шейдер %s\n", fFileName );
		GXDebugBox ( L"CreateShaderProgram::Error - Фрагментный шейдер не скомпилировался" );
	}

	glAttachShader ( shaderProgram, vertexShader );
	glDeleteShader ( vertexShader );

	if ( geometryShader )
	{
		glAttachShader ( shaderProgram, geometryShader );
		glDeleteShader ( geometryShader );
	}

	glAttachShader ( shaderProgram, fragmentShader );
	glDeleteShader ( fragmentShader );

	glLinkProgram ( shaderProgram );
	if ( GXShaderProgramStatus ( shaderProgram, GL_LINK_STATUS, vFileName, gFileName, fFileName ) != GL_TRUE )
	{
		GXLogW ( L"GXCreateShaderProgram::Error - Не могу собрать шейдер %s + %s + %s\n", vFileName, gFileName, fFileName );
		GXDebugBox ( L"CreateShaderProgram::Error - Шейдерная программа не собралась" );
	}

	#ifdef DEBUG_PAUSE_AFTER_SHADERS
		system ( "pause" );
	#endif

	return shaderProgram;
}

GLuint GXCALL GXCreateTransformFeedbackShaderProgram ( const GXWChar* vFileName, const GXWChar* gFileName, const GXWChar* fFileName, GXUByte numVaryings, const GLchar** varyings, GLenum bufferMode )
{
	GLuint shaderProgram = glCreateProgram ();
	GLuint vertexShader = glCreateShader ( GL_VERTEX_SHADER );

	GXChar* shaderSource;
	GXUInt shaderSourceLength;

	if ( !GXLoadFile ( vFileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
	{
		GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу загрузить вершинный шейдер %s\n", vFileName );
		GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Вершинный шейдер не загрузился" );
	}

	glShaderSource ( vertexShader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
	glCompileShader ( vertexShader );
	GXSafeFree ( shaderSource );

	if ( GXShaderCompilationStatus ( vertexShader, vFileName ) != GL_TRUE )
	{
		GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу скомпилировать вершинный шейдер %s\n", vFileName );
		GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Вершинный шейдер не скомпилировался" );
	}

	GLuint geometryShader = 0;
	if ( gFileName )
	{
		if ( !GXLoadFile ( gFileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
		{
			GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу загрузить геометрический шейдер %s\n", gFileName );
			GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Геометрический шейдер не загрузился" );
		}

		geometryShader = glCreateShader ( GL_GEOMETRY_SHADER );

		glShaderSource ( geometryShader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
		glCompileShader ( geometryShader );
		GXSafeFree ( shaderSource );

		if ( GXShaderCompilationStatus ( geometryShader, gFileName ) != GL_TRUE )
		{
			GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу скомпилировать геометрический шейдер %s\n", gFileName );
			GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Геометрический шейдер не скомпилировался" );
		}
	}

	if ( !GXLoadFile ( fFileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
	{
		GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу загрузить фрагментный шейдер %s\n", fFileName );
		GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Фрагментный шейдер не загрузился" );
	}

	GLuint fragmentShader = glCreateShader ( GL_FRAGMENT_SHADER );
	glShaderSource ( fragmentShader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
	glCompileShader ( fragmentShader );
	GXSafeFree ( shaderSource );

	if ( GXShaderCompilationStatus ( fragmentShader, fFileName ) != GL_TRUE )
	{
		GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу скомпилировать фрагментный шейдер %s\n", fFileName );
		GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Фрагментный шейдер не скомпилировался" );
	}

	glAttachShader ( shaderProgram, vertexShader );
	glDeleteShader ( vertexShader );

	if ( geometryShader )
	{
		glAttachShader ( shaderProgram, geometryShader );
		glDeleteShader ( geometryShader );
	}

	glAttachShader ( shaderProgram, fragmentShader );
	glDeleteShader ( fragmentShader );

	glTransformFeedbackVaryings ( shaderProgram, numVaryings, varyings, bufferMode );
	glLinkProgram ( shaderProgram );
	if ( GXShaderProgramStatus ( shaderProgram, GL_LINK_STATUS, vFileName, gFileName, fFileName ) != GL_TRUE )
	{
		GXLogW ( L"GXCreateTransformFeedbackShaderProgram::Error - Не могу собрать шейдер %s + %s + %s\n", vFileName, gFileName, fFileName );
		GXDebugBox ( L"GXCreateTransformFeedbackShaderProgram::Error - Шейдерная программа не собралась" );
	}

	#ifdef DEBUG_PAUSE_AFTER_SHADERS
		system ( "pause" );
	#endif

	return shaderProgram;
}

GLint GXCALL GXGetUniformLocation ( GLint shaderProgram, const GXChar* uniform )
{
	GLint b = glGetUniformLocation ( shaderProgram, uniform );

	#ifdef DEBUG_ERROR_AND_SHUTDOWN
		assert ( b + 1 );
	#endif

	if ( b == -1 )
	{
		#ifdef DEBUG_WARNING_AND_PAUSE
			GXLogW ( L"GetUniformLocation::Error - Не найду юниформы: ", uniform );
			GXLogA ( "%s\n", uniform );
			system ( "pause" );
		#endif
	}

	return b;
}
