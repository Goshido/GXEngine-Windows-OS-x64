//vesrion 1.4

#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>


GXAVLTreeNode::GXAVLTreeNode ()
{
	left = right = 0;
	height = 1;
}

GXAVLTreeNode::~GXAVLTreeNode ()
{
	//PURE VIRTUAL
}

//----------------------------------------------------------------------------------------------

GXUInt GXAVLTree::GetTotalNodes () const
{
	return totalNodes;
}

GXAVLTree::GXAVLTree ( PFNGXAVLTREECOMPAREPROC comparator, GXBool isAutoClean )
{
	Compare = comparator;
	this->isAutoClean = isAutoClean;
	root = nullptr;
	totalNodes = 0;
}

GXAVLTree::~GXAVLTree ()
{
	if ( isAutoClean )
		DeleteTree ( root );
}

const GXAVLTreeNode* GXAVLTree::Find ( const GXAVLTreeNode &node ) const
{
	GXAVLTreeNode* p = root;

	while ( p )
	{
		GXInt compareResult = Compare ( node, *p );

		if ( compareResult < 0 )
			p = p->left;
		else if ( compareResult > 0 )
			p = p->right;
		else
			return p;
	}
	
	return nullptr;
}

GXVoid GXAVLTree::Add ( GXAVLTreeNode &node )
{
	if ( Find ( node ) ) return;

	root = Insert ( &node, root );
	totalNodes++;
}

GXVoid GXAVLTree::DoPrefix ( const GXAVLTreeNode* root, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const
{
	if ( !root ) return;

	iterator ( *root, args );
	DoPrefix ( root->left, iterator, args );
	DoPrefix ( root->right, iterator, args );
}

GXVoid GXAVLTree::DoInfix ( const GXAVLTreeNode* root, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const
{
	if ( !root ) return;

	DoInfix ( root->left, iterator, args );
	iterator ( *root, args );
	DoInfix ( root->right, iterator, args );
}

GXVoid GXAVLTree::DoPostfix ( const GXAVLTreeNode* root, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const
{
	if ( !root ) return;

	DoPostfix ( root->left, iterator, args );
	DoPostfix ( root->right, iterator, args );
	iterator ( *root, args );
}

GXUInt GXAVLTree::GetHeight ( GXAVLTreeNode* node ) const
{
	return node ? node->height : 0;
}

GXInt GXAVLTree::GetBalance ( GXAVLTreeNode* node )
{
	return GetHeight ( node->right ) - GetHeight ( node->left );
}

GXAVLTreeNode* GXAVLTree::LeftRotate ( GXAVLTreeNode* father )
{
	GXAVLTreeNode* me = father->right;

	father->right = me->left;
	me->left = father;

	FixHeight ( father );
	FixHeight ( me );

	return me;
}

GXAVLTreeNode* GXAVLTree::RightRotate ( GXAVLTreeNode* father )
{
	GXAVLTreeNode* me = father->left;

	father->left = me->right;
	me->right = father;

	FixHeight ( father );
	FixHeight ( me );

	return me;
}

GXAVLTreeNode* GXAVLTree::Insert ( GXAVLTreeNode* node, GXAVLTreeNode* root )
{
	if ( !root ) return node;

	if ( Compare ( *node, *root ) < 0 )
		root->left = Insert ( node, root->left );
	else
		root->right = Insert ( node, root->right );

	return Balance ( root );
}

GXVoid GXAVLTree::FixHeight ( GXAVLTreeNode* node )
{
	GXUInt lh = GetHeight ( node->left );
	GXUInt rh = GetHeight ( node->right );

	node->height = 1 + ( lh > rh ? lh : rh );
}

GXAVLTreeNode* GXAVLTree::Balance ( GXAVLTreeNode* me )
{
	FixHeight ( me );

	GXChar difference = GetBalance ( me );

	if ( difference == 2 )
	{
		GXAVLTreeNode* rightSon = me->right;

		if ( GetBalance ( rightSon ) < 0 )
			me->right = RightRotate ( rightSon );

		return LeftRotate ( me );
	}

	if ( difference == -2 )
	{
		GXAVLTreeNode* leftSon = me->left;

		if ( GetBalance ( leftSon ) > 0 )
			me->left = LeftRotate ( leftSon );

		return RightRotate ( me );
	}

	return me;
}

GXVoid GXAVLTree::DeleteTree ( GXAVLTreeNode* node )
{
	if ( node )
	{
		DeleteTree ( node->left );
		DeleteTree ( node->right );
		delete node;
	}
}