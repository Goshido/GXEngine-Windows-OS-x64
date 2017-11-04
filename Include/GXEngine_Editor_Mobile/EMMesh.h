#ifndef EM_MESH
#define EM_MESH


#include <GXEngine/GXTransform.h>
#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXMeshGeometry.h>


class EMMesh : public GXTransform, public GXRenderable
{
	private:
		GXMeshGeometry	meshGeometry;
		GXWChar*		meshFileName;
		GXWChar*		skinFileName;
		GXAABB			boundsWorld;

	public:
		EMMesh ( const GXWChar* meshFileName );
		EMMesh ( const GXWChar* meshFileName, const GXWChar* skinFileName );
		~EMMesh () override;

		GXVoid Render () override;

		GXVoid UpdatePose ( GXSkeleton &skeleton );

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid TransformUpdated () override;
};


#endif //EM_MESH
