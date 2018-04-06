//version 1.0

#include <GXPhysics/GXSparseMatrix.h>


GXSparseMatrixElement::GXSparseMatrixElement ()
{
	// NOTHING
}

GXSparseMatrixElement::GXSparseMatrixElement ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 )
{
	data[ 0 ] = a1;
	data[ 1 ] = a2;
	data[ 2 ] = a3;
	data[ 3 ] = a4;
	data[ 4 ] = a5;
	data[ 5 ] = a6;
}

GXVoid GXSparseMatrixElement::Init ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 )
{
	data[ 0 ] = a1;
	data[ 1 ] = a2;
	data[ 2 ] = a3;
	data[ 3 ] = a4;
	data[ 4 ] = a5;
	data[ 5 ] = a6;
}

GXFloat GXSparseMatrixElement::DotProduct ( const GXSparseMatrixElement &other ) const
{
	return data[ 0 ] * other.data[ 0 ] + data[ 1 ] * other.data[ 1 ] + data[ 2 ] * other.data[ 2 ] + data[ 3 ] * other.data[ 3 ] + data[ 4 ] * other.data[ 4 ] + data[ 5 ] * other.data[ 5 ];
}

GXVoid GXSparseMatrixElement::Sum ( const GXSparseMatrixElement &a, const GXSparseMatrixElement &b )
{
	data[ 0 ] = a.data[ 0 ] + b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] + b.data[ 3 ];
	data[ 4 ] = a.data[ 4 ] + b.data[ 4 ];
	data[ 5 ] = a.data[ 5 ] + b.data[ 5 ];
}

GXVoid GXSparseMatrixElement::Multiply ( const GXSparseMatrixElement &a, GXFloat factor )
{
	data[ 0 ] = a.data[ 0 ] * factor;
	data[ 1 ] = a.data[ 1 ] * factor;
	data[ 2 ] = a.data[ 2 ] * factor;
	data[ 3 ] = a.data[ 3 ] * factor;
	data[ 4 ] = a.data[ 4 ] * factor;
	data[ 5 ] = a.data[ 5 ] * factor;
}

GXSparseMatrixElement& GXSparseMatrixElement::operator = ( const GXSparseMatrixElement &other )
{
	memcpy ( this, &other, sizeof ( GXSparseMatrixElement ) );
}
