//version 1.0

#ifndef GX_SPARSE_MATRIX
#define GX_SPARSE_MATRIX


#include <GXCommon/GXTypes.h>


struct GXSparseMatrixElement
{
	GXFloat		data[ 6 ];

	GXSparseMatrixElement ();
	explicit GXSparseMatrixElement ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 );

	GXVoid Init ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 );

	GXFloat DotProduct ( const GXSparseMatrixElement &other ) const;
	GXVoid Sum ( const GXSparseMatrixElement &a, const GXSparseMatrixElement &b );
	GXVoid Multiply ( const GXSparseMatrixElement &a, GXFloat factor );

	GXSparseMatrixElement& operator = ( const GXSparseMatrixElement &other );
};


#endif //GX_SPARSE_MATRIX
