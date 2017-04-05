#ifndef EM_MESH
#define EM_MESH


#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXMeshGeometry.h>


class EMMesh : public GXRenderable
{
	private:
		GXMeshGeometry	meshGeometry;
		GXWChar*		meshFileName;
		GXAABB			boundsWorld;

	public:
		EMMesh ( const GXWChar* staticMeshFileName );
		~EMMesh () override;

		GXVoid Render () override;

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid UpdateBounds () override;
};


#endif //EM_MESH
