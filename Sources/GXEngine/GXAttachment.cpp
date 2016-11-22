//version 1.0

#include <GXEngine/GXAttachment.h>
#include <cstdlib>


GXAttachment::GXAttachment ( const GXWChar* name, GXMesh* parent, const GXMat4 &offset )
{
	GXUInt size = (GXUInt)( sizeof ( GXWChar ) * ( wcslen ( name ) + 1 ) );
	this->name = (GXWChar*)malloc ( size );
	memcpy ( this->name, name, size );

	this->parent = parent;
	this->offset = offset;
}

GXAttachment::~GXAttachment ()
{
	free ( name );
}

GXVoid GXAttachment::GetTransform ( GXVec3 &location, GXMat4 &rotation )
{
	GXMat4 parent_mod_mat;
	parent->GetRotation ( parent_mod_mat );
	parent->GetLocation ( parent_mod_mat.wv );

	GXVec3 scale;
	parent->GetScale ( scale );

	GXMat4 tmp;
	tmp = offset;
	tmp.wv.x = offset.wv.x * scale.x;
	tmp.wv.y = offset.wv.y * scale.y;
	tmp.wv.z = offset.wv.z * scale.z;

	GXMulMat4Mat4 ( rotation, tmp, parent_mod_mat );

	location = rotation.wv;
	rotation.wv = GXVec3 ( 0.0f, 0.0f, 0.0f ); 
}

const GXWChar* GXAttachment::GetName ()
{
	return name;
}

GXVoid GXAttachment::GetOffset ( GXMat4 &offset )
{
	offset = this->offset;
}

GXVoid GXAttachment::SetOffset ( const GXMat4 &offset )
{
	this->offset = offset;
}