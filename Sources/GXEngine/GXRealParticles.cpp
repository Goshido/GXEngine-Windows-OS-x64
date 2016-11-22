//version 1.0

#include <GXEngine/GXRealParticles.h>
#include <GXEngine/GXGlobals.h>
#include <time.h>


GXRealParticles::GXRealParticles ( GXParticleSystemParams &psParams )
{
	Load3DModel ();
	InitUniforms ();
	
	srand ( (GXUInt)time ( 0 ) );

	maxParticles = psParams.maxParticles;
	lifespan = psParams.lifespan;
	lifespanDeviation = psParams.lifespanDeviation;
	SetVelocityModulus ( psParams.startVelocity.x, psParams.startVelocity.y, psParams.startVelocity.z );

	GXPhysics* physics = gx_Core->GetPhysics ();
	if ( !( particleSystem = physics->CreateParticleSystem ( maxParticles, GX_FALSE ) ) )
	{
		GXDebugBox ( L"Система частиц не создана\n" );
		GXLogW ( L"GXRealParticles::GXRealParticles::Error - Система частиц не создана\n" );
	}

	particleSystem->setExternalAcceleration ( PxVec3 ( psParams.perFrameAcceleration.x, psParams.perFrameAcceleration.y, psParams.perFrameAcceleration.z ) );
	particleSystem->setMaxMotionDistance ( psParams.maxPerFrameDistance );

	particlesInd = (PxU32*)malloc ( maxParticles * sizeof ( PxU32 ) );
	particlesPos = (PxVec3*)malloc ( maxParticles * sizeof ( PxVec3 ) );
	particlesVel = (PxVec3*)malloc ( maxParticles * sizeof ( PxVec3 ) );

	newPaticlesInd = (PxU32*)malloc ( maxParticles * sizeof ( PxU32 ) );
	newPaticlesVel = (PxVec3*)malloc ( maxParticles * sizeof ( PxVec3 ) );
	newPaticlesPos = (PxVec3*)malloc ( maxParticles * sizeof ( PxVec3 ) );

	particlesLifeLeft = (GXFloat*)malloc ( maxParticles * sizeof ( GXFloat ) );
	particlesSize = (GXVec3*)malloc ( maxParticles * sizeof ( GXVec3 ) );

	UpdateParticleSystem ();

	PxParticleCreationData cd;
	cd.numParticles = maxParticles;
	cd.indexBuffer = PxStrideIterator<const PxU32> ( particlesInd );
	cd.positionBuffer = PxStrideIterator<const PxVec3> ( particlesPos );
	cd.velocityBuffer = PxStrideIterator<const PxVec3> ( particlesVel );

	if ( !particleSystem->createParticles ( cd ) )
	{
		GXDebugBox ( L"Система частиц не настроена\n" );
		GXLogW ( L"GXRealParticles::GXRealParticles::Error - Система частиц не настроена\n" );
	}

	physics->AddParticleSystem ( *particleSystem );
}

GXRealParticles::~GXRealParticles ()
{
	free ( particlesInd );
	free ( particlesPos );
	free ( particlesVel );

	free ( particlesLifeLeft );
	free ( particlesSize );

	free ( newPaticlesInd );
	free ( newPaticlesPos );
	free ( newPaticlesVel );

	GXRemoveVAO ( vaoInfo );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid GXRealParticles::Draw ()
{
	GXFloat deltatime = delta * 0.001f;
	
	glEnable ( GL_BLEND );
	glDepthMask ( GL_FALSE );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );
	glBindVertexArray ( vaoInfo.vao );

	PxParticleReadData* rd = particleSystem->lockParticleReadData ();

	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 cam_mod_mat;
	cam_mod_mat = gx_ActiveCamera->GetModelMatrix ();
	GXMat4 mod_view_proj_mat;

	if ( rd )
	{
		PxStrideIterator<const PxParticleFlags> flagsIt ( rd->flagsBuffer );
		PxStrideIterator<const PxVec3> positionIt ( rd->positionBuffer );

		GXUInt counter = 0;
		for ( GXUInt i = 0; i < rd->validParticleRange; i++, flagsIt++, positionIt++ )
		{
			if ( *flagsIt & PxParticleFlag::eVALID )
			{
				GXSetMat4TranslateTo ( cam_mod_mat, *( (GXVec3*)positionIt.ptr () ) );

				GXMulMat4Mat4 ( mod_view_proj_mat, cam_mod_mat, *view_proj_mat );

				glUniformMatrix4fv ( mod_view_proj_mat_Location, 1, GL_TRUE, mod_view_proj_mat.A );
				glUniform3fv ( particle_size_Location, 1, (GLfloat*)( particlesSize + i ) );
				glUniform1fv ( life_left_Location, 1, particlesLifeLeft + i );

				glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

				if ( ( particlesLifeLeft[ i ] -= deltatime ) <= 0.0f )
				{
					particlesLifeLeft[ i ] = lifespan + (GXFloat)rand () * 3.05185095e-5f * lifespanDeviation;
					particlesSize[ i ] = GXCreateVec3 ( scale_mat.m11, scale_mat.m22, scale_mat.m33 );
					newPaticlesInd[ counter ] = i;
					newPaticlesPos[ counter ] = PxVec3 ( mod_mat.m41, mod_mat.m42, mod_mat.m43 );
					newPaticlesVel[ counter++ ] = PxVec3 ( particlesVel[ i ].x * startVelocity.x, particlesVel[ i ].y * startVelocity.y, particlesVel[ i ].z * startVelocity.z );	
				}
			}
		}

		if ( counter )
		{
			rd->unlock();

			particleSystem->setPositions ( counter, PxStrideIterator<const PxU32> ( newPaticlesInd ), PxStrideIterator<const PxVec3> ( newPaticlesPos ) );
			particleSystem->setVelocities ( counter, PxStrideIterator<const PxU32> ( newPaticlesInd ), PxStrideIterator<const PxVec3> ( newPaticlesVel ) );

			rd = particleSystem->lockParticleReadData ();
		}

		rd->unlock();
	}

	glUseProgram ( 0 );
	glDepthMask ( GL_TRUE );
	glDisable ( GL_BLEND );
}

GXVoid GXRealParticles::SetVelocityModulus ( GXFloat x, GXFloat y, GXFloat z )
{
	startVelocity = GXCreateVec3 ( x, y, z );
}

GXVoid GXRealParticles::Simulate ( GXDword delta )
{
	this->delta = delta;
}

GXVoid GXRealParticles::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/System/Particle.mtr", mat_info );

	GXGetTexture ( mat_info.textures [ 0 ] );
	GXGetShaderProgram ( mat_info.shaders [ 0 ] );
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
}

GXVoid GXRealParticles::InitUniforms ()
{
	glUseProgram ( mat_info.shaders [ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "textureID" ), 0 );
	mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "mod_view_proj_mat" );
	particle_size_Location = GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "particle_size" );
	life_left_Location = GXGetUniformLocation ( mat_info.shaders [ 0 ].uiId, "life_left" );

	glUseProgram ( 0 );
}

GXVoid GXRealParticles::UpdateParticleSystem ()
{
	GXFloat buf = sqrtf ( (GXFloat)maxParticles );
	GXFloat angleStepAsimuth = 2.0f * GX_MATH_PI / (GXFloat)( (GXUInt)buf );
	GXFloat angleStepZenith = angleStepAsimuth * 0.5f;
	GXUInt ind = 0;
	for ( GXFloat asimuth = 0.0f; asimuth < 2.0f * GX_MATH_PI; asimuth += angleStepAsimuth )
	{
		for ( GXFloat zenith = -GX_MATH_HALFPI; zenith < GX_MATH_HALFPI; zenith += angleStepZenith, ind++ )
		{
			GXFloat s = sinf ( asimuth);
			particlesVel[ ind ].x = s * cosf ( zenith );
			particlesVel[ ind ].y = cosf ( asimuth );
			particlesVel[ ind ].z = s * sinf ( zenith );
		}
	}

	GXUInt uninit = maxParticles - ( (GXUInt)buf * (GXUInt)buf );
	for ( GXUInt i = 0; i < uninit; i++ )
	{
		particlesVel[ maxParticles - i - 1 ].x = particlesVel[ i ].x;
		particlesVel[ maxParticles - i - 1 ].y = particlesVel[ i ].y;
		particlesVel[ maxParticles - i - 1 ].z = particlesVel[ i ].z;
	}

	for ( GXUInt i = 0; i< maxParticles; i++ )
	{
		particlesInd[ i ] = i;
		memcpy ( particlesPos, mod_mat.wv.v, sizeof ( GXVec3 ) );
	}

	memset ( particlesLifeLeft, (GXInt)( ( (GXInt*)( &lifespan ) )[ 0 ] ), maxParticles * sizeof ( GXFloat ) );
}

//-------------------------------------------------------------------------------------------------------

