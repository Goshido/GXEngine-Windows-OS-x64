//version 1.3

#include <GXEngine/GXIdealParticles.h>
#include <time.h>


GXIdealParticles::GXIdealParticles ( GXIdealParticleSystemParams &psParams )
{
	memcpy ( &params, &psParams, sizeof ( GXIdealParticleSystemParams ) );

	for ( GXUChar i = 0; i < 2; i++ )
	{
		vao[ i ] = 0;
		position[ i ] = 0;
		velocity[ i ] = 0;
		lifetime[ i ] = 0;
	}

	distribution = 0;

	deltaTime = 0.0f;
	writeBuffer = 0;
	invLifeTime = 1.0f / params.lifeTime;

	isReseted = GX_TRUE;
	isDelete = GX_FALSE;
}

GXVoid GXIdealParticles::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid GXIdealParticles::Draw ()
{
	isReseted = GX_FALSE;
}

GXVoid GXIdealParticles::Update ( GXFloat deltaTime )
{
	this->deltaTime = deltaTime;
}

GXVoid GXIdealParticles::Reset ()
{
	if ( isReseted ) return;

	GXUChar readBuffer = writeBuffer == 0 ? 1 : 0;

	glBindBuffer ( GL_ARRAY_BUFFER, position[ readBuffer ] );
	//{
		glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXVec3 ), params.startPositions, GL_DYNAMIC_DRAW );
	//}

	glBindBuffer ( GL_ARRAY_BUFFER, velocity[ readBuffer ] );
	//{
		glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXVec3 ), params.startVelocities, GL_DYNAMIC_DRAW );
	//}

	glBindBuffer ( GL_ARRAY_BUFFER, lifetime[ readBuffer ] );
	//{
		glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXFloat ), params.startLifeTime, GL_DYNAMIC_DRAW );
	//}

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	isReseted = GX_TRUE;
}

GXIdealParticles::~GXIdealParticles ()
{
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers ( 2, position );
	glDeleteBuffers ( 2, velocity );
	glDeleteBuffers ( 2, lifetime );
	glDeleteBuffers ( 1, &distribution );

	free ( params.distribution );
	free ( params.startLifeTime );
	free ( params.startPositions );
	free ( params.startVelocities );

	glBindVertexArray ( 0 );
	glDeleteVertexArrays ( 2, vao );
}

GXVoid GXIdealParticles::InitGraphicalResources ()
{
	glGenVertexArrays ( 2, vao );
	glGenBuffers ( 2, position );
	glGenBuffers ( 2, velocity );
	glGenBuffers ( 2, lifetime );

	glGenBuffers ( 1, &distribution );
	glBindBuffer ( GL_ARRAY_BUFFER, distribution );
	glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXVec3 ), params.distribution, GL_DYNAMIC_DRAW );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	for ( GXUChar i = 0; i < 2; i++ )
	{
		glBindVertexArray ( vao[ i ] );
		//{
			glBindBuffer ( GL_ARRAY_BUFFER, position[ i ] );
			//{
				glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXVec3 ), params.startPositions, GL_DYNAMIC_DRAW );
				glVertexAttribPointer ( GX_IDEAL_PARTICLES_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec3 ), 0 );
				glEnableVertexAttribArray ( GX_IDEAL_PARTICLES_POSITION );
			//}

			glBindBuffer ( GL_ARRAY_BUFFER, velocity[ i ] );
			//{
				glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXVec3 ), params.startVelocities, GL_DYNAMIC_DRAW );
				glVertexAttribPointer ( GX_IDEAL_PARTICLES_VELOCITY, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec3 ), 0 );
				glEnableVertexAttribArray ( GX_IDEAL_PARTICLES_VELOCITY );
			//}

			glBindBuffer ( GL_ARRAY_BUFFER, lifetime[ i ] );
			//{
				glBufferData ( GL_ARRAY_BUFFER, params.maxParticles * sizeof ( GXFloat ), params.startLifeTime, GL_DYNAMIC_DRAW );
				glVertexAttribPointer ( GX_IDEAL_PARTICLES_LIFETIME, 1, GL_FLOAT, GL_FALSE, sizeof ( GXFloat ), 0 );
				glEnableVertexAttribArray ( GX_IDEAL_PARTICLES_LIFETIME );
			//}

			glBindBuffer ( GL_ARRAY_BUFFER, distribution );
			//{
				glVertexAttribPointer ( GX_IDEAL_PARTICLES_DISTRIBUTION, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec3 ), 0 );
				glEnableVertexAttribArray ( GX_IDEAL_PARTICLES_DISTRIBUTION );
			//}
		//}
	}

	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}

GXVoid GXIdealParticles::SwapTransformFeedbackBuffers ()
{
	writeBuffer = writeBuffer ? 0 : 1;
}