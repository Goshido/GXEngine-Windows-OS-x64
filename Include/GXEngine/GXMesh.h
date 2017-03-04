//version 1.10

#ifndef GX_MESH
#define GX_MESH


#include "GXDrawable.h"
#include "GXVAOStorage.h"
#include "GXTextureStorage.h"
#include "GXShaderStorage.h"
#include "GXShaderUtils.h"
#include <GXCommon/GXMath.h>


class GXMesh : public GXDrawable
{
	protected:
		GXVAOInfo			vaoInfo;

		GXMat4				trans_mat;
		GXMat4				rot_mat;
		GXMat4				scale_mat;
		GXMat4				scale_rot_mat;
		GXMat4				mod_mat;

		GXUByte				numPrograms;
		GXShaderProgram*	programs;

		GXUByte				numTextures;
		GXTexture*			textures;

		GXAABB				boundsWorld;

	public:
		GXMesh ();
		virtual ~GXMesh ();

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetLocation ( const GXVec3 &loc );

		virtual GXVoid SetRotation ( GXFloat* Rot_rad );
		virtual GXVoid SetRotation ( const GXVec3 &rot_rad );
		virtual GXVoid SetRotation ( const GXMat4 &rot );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		virtual GXVoid SetRotation ( const GXQuat &q );

		virtual GXVoid SetScale ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetScale ( GXFloat* Scl );
		virtual GXVoid SetScale ( const GXVec3 &scale );

		virtual GXVoid GetLocation ( GXFloat* x, GXFloat* y, GXFloat* z );
		virtual GXVoid GetLocation ( GXVec3 &loc );

		virtual GXVoid GetRotation ( GXMat4 &rot );
		virtual GXVoid GetRotation ( GXQuat &rot );

		virtual GXVoid GetScale ( GXFloat* x, GXFloat* y, GXFloat* z );
		virtual GXVoid GetScale ( GXVec3 &scale );

		virtual GXVoid Draw () = 0;

		virtual const GXVAOInfo& GetMeshVAOInfo ();
		virtual const GXAABB& GetBoundsWorld ();
		virtual const GXMat4& GetModelMatrix ();

		virtual GXBool IsVisible ();

	protected:
		virtual GXVoid Load3DModel () = 0;
		virtual GXVoid InitUniforms () = 0;
		GXVoid UpdateBounds ();
		GXVoid AllocateShaderPrograms ( GXUByte numPrograms );
		GXVoid AllocateTextures ( GXUByte numTextures );
};

#endif //GX_MESH
