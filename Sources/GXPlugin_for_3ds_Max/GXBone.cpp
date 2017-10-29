//version 1.1

#include <GXPlugin_for_3ds_Max/GXBone.h>
#include <GXCommon/GXStrings.h>


GXVoid GXBone::Init ( GXUShort newOwnIndex, IGameNode* newNode, const GXMat4 &newTransformWorld )
{
	GXUTF8* s = nullptr;
	GXToUTF8 ( &s, newNode->GetName () );
	GXUInt size = GXUTF8size ( s );

	if ( size > GX_BONE_NAME_SIZE * sizeof ( GXUTF8 ) )
	{
		memcpy ( name, s, ( GX_BONE_NAME_SIZE - 1 ) * sizeof ( GXUTF8 ) );
		name[ GX_BONE_NAME_SIZE - 1 ] = 0;
	}
	else
	{
		memcpy ( name, s, size );
	}

	free ( s );

	ownIndex = newOwnIndex;
	parentIndex = GX_UNKNOWN_BONE_INDEX;
	isRootBone = GX_FALSE;
	node = newNode;
	transformWorld = newTransformWorld;

	father = brother = son = nullptr;
}

GXVoid GXBone::AddSon ( GXBone* newSon )
{
	newSon->father = this;

	if ( !son )
	{
		son = newSon;
		return;
	}

	GXBone* youngerSon = son;

	while ( GX_TRUE )
	{
		if ( !youngerSon->brother )
		{
			//Son has come home.
			youngerSon->brother = newSon;
			return;
		}
		else if ( youngerSon->node == newSon->node )
		{
			//Son is home already.
			return;
		}

		youngerSon = youngerSon->brother;
	}
}

GXBone& GXBone::operator = ( const GXBone &other )
{
	memcpy ( this, &other, sizeof ( GXBone ) );
	return *this;
}
