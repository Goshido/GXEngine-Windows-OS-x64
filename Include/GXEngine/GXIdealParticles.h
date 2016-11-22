//version 1.3

#ifndef GX_IDEAL_PARTICLES
#define GX_IDEAL_PARTICLES


#include "GXParticleSystem.h"
#include "GXMesh.h"


#define GX_IDEAL_PARTICLES_POSITION		0
#define GX_IDEAL_PARTICLES_VELOCITY		1
#define GX_IDEAL_PARTICLES_LIFETIME		2
#define GX_IDEAL_PARTICLES_DISTRIBUTION	3


class GXIdealParticles : public GXMesh
{
	protected:
		GXIdealParticleSystemParams params;

		GLuint						vao[ 2 ];
		GLuint						position[ 2 ];
		GLuint						velocity[ 2 ];
		GLuint						lifetime[ 2 ];
		GLuint						distribution;

		GXFloat						deltaTime;
		GXFloat						invLifeTime;

		GXUChar						writeBuffer;

		GXBool						isReseted;
		GXBool						isDelete;

	public:
		GXIdealParticles ( GXIdealParticleSystemParams &psParams );
		GXVoid Delete ();

		virtual GXVoid Draw ();
		virtual GXVoid Update ( GXFloat deltaTime );

		GXVoid Reset ();		

	protected:
		virtual ~GXIdealParticles ();

		virtual GXVoid Load3DModel () = 0;
		virtual GXVoid InitUniforms () = 0;

		virtual GXVoid InitGraphicalResources ();	//Обязательно вызывать в потомках

		GXVoid SwapTransformFeedbackBuffers();
};


#endif	//GX_IDEAL_PARTICLES