//version 1.1

#ifndef GX_MESH_NORMAL_HOLDER
#define GX_MESH_NORMAL_HOLDER


#include <GXCommon/GXMath.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <max.h>
#include <GXCommon/GXRestoreWarnings.h>


class GXVertexNormal;
class GXMeshNormalHolder
{
	private:
		Tab<GXVertexNormal>		normals;
		Mesh&					mesh;

	public:
		explicit GXMeshNormalHolder ( Mesh &mesh );
		~GXMeshNormalHolder ();

		GXVoid BuildNormals ();
		GXVoid BuildStrips ();
		GXVoid GetNormal ( GXVec3 &outNormal, GXUInt index, GXDword smoothGroup ) const;

	private:
		GXMeshNormalHolder ( const GXMeshNormalHolder &other ) = delete;
		GXMeshNormalHolder& operator = ( const GXMeshNormalHolder &other ) = delete;
};


#endif //GX_MESH_NORMAL_HOLDER
