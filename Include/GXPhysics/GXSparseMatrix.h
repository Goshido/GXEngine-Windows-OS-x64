//version 1.0

#ifndef GX_SPARSE_MATRIX
#define GX_SPARSE_MATRIX


#include <GXCommon/GXTypes.h>


struct GXSparseMatrixElement
{
	GXFloat		data[ 6 ];

	GXFloat DotProduct ( const GXSparseMatrixElement &other ) const;
	GXVoid Sum ( const GXSparseMatrixElement &a, const GXSparseMatrixElement &b );
};

//-------------------------------------------------------------

struct GXSparseMatrix
{
	GXSparseMatrixElement*		elements;
	GXUByte*					map;
};


#endif //GX_SPARSE_MATRIX
