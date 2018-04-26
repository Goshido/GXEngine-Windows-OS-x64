#ifndef EM_MESH
#define EM_MESH


#include <GXEngine/GXTransform.h>
#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXMeshGeometry.h>


class EMMesh : public GXTransform, public GXRenderable
{
	private:
		GXWChar*		skinFileName;
		GXWChar*		meshFileName;
		GXMeshGeometry	meshGeometry;
		GXAABB			boundsWorld;

	public:
		explicit EMMesh ( const GXWChar* meshFileName );
		explicit EMMesh ( const GXWChar* meshFileName, const GXWChar* skinFileName );
		~EMMesh () override;

		GXVoid Render () override;

		GXVoid UpdatePose ( GXSkeleton &skeleton );

	protected:
		GXVoid InitGraphicResources () override;
		GXVoid TransformUpdated () override;

	private:
		EMMesh () = delete;
		EMMesh ( const EMMesh &other ) = delete;
		EMMesh& operator = ( const EMMesh &other ) = delete;
};


#endif // EM_MESH
