//version 1.0

#ifndef GX_ATTACHMENT
#define GX_ATTACHMENT


#include "GXMesh.h"


class GXAttachment
{
	private:
		GXMesh*		parent;
		GXMat4		offset;
		GXWChar*	name;

	public:
		GXAttachment ( const GXWChar* name, GXMesh* parent, const GXMat4 &offset );
		~GXAttachment ();

		GXVoid GetTransform ( GXVec3 &location, GXMat4 &rotation );
		const GXWChar* GetName ();

		GXVoid GetOffset ( GXMat4 &offset );
		GXVoid SetOffset ( const GXMat4 &offset );
};


#endif //GX_ATTACHMENT