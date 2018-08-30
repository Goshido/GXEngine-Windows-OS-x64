// vesrion 1.6


#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


GXAVLTreeNode::GXAVLTreeNode ()
{
	left = right = nullptr;
	height = 1u;
}

GXAVLTreeNode::~GXAVLTreeNode ()
{
	// NOTHING
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
	GXAVLTreeNode* parentNode = nullptr;
	eGXAVLTreeSide side = eGXAVLTreeSide::Unknown;
	GXAVLTreeNode* oldNode;

	FindInternal ( &oldNode, &parentNode, side, node );

	if ( oldNode )
	{
		if ( isAutoClean )
		{
			if ( !parentNode )
			{
				root = &node;
			}
			else
			{
				switch ( side )
				{
					case eGXAVLTreeSide::Left:
						parentNode->left = &node;
					break;

					case eGXAVLTreeSide::Right:
						parentNode->right = &node;
					break;

					case eGXAVLTreeSide::Unknown:
						GXLogW ( L"GXAVLTree::Add::Error - Как код попал в ветку eGXAVLTreeSide::Unknown?\n" );
					break;

					default:
						// NOTHING
					break;
				}
			}

			node.left = oldNode->left;
			node.right = oldNode->right;
			node.height = oldNode->height;

			delete oldNode;
		}
		else
		{
			GXLogW ( L"GXAVLTree::Add::Error - Обнаружен элемент с идентичным ключом. Не могу выполнить обмен элементов, так как требуется удаление старого элемента, а режим авточистки запрещён.\n" );
		}
	}
	else
	{
		root = Insert ( &node, root );
		++totalNodes;
	}
}

GXVoid GXAVLTree::DoPrefix ( const GXAVLTreeNode* currentRoot, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const
{
	if ( !currentRoot ) return;

	iterator ( *currentRoot, args );
	DoPrefix ( currentRoot->left, iterator, args );
	DoPrefix ( currentRoot->right, iterator, args );
}

GXVoid GXAVLTree::DoInfix ( const GXAVLTreeNode* currentRoot, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const
{
	if ( !currentRoot ) return;

	DoInfix ( currentRoot->left, iterator, args );
	iterator ( *currentRoot, args );
	DoInfix ( currentRoot->right, iterator, args );
}

GXVoid GXAVLTree::DoPostfix ( const GXAVLTreeNode* currentRoot, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const
{
	if ( !currentRoot ) return;

	DoPostfix ( currentRoot->left, iterator, args );
	DoPostfix ( currentRoot->right, iterator, args );
	iterator ( *currentRoot, args );
}

GXVoid GXAVLTree::FindInternal ( GXAVLTreeNode** oldNode, GXAVLTreeNode** parent, eGXAVLTreeSide &side, const GXAVLTreeNode &node )
{
	*parent = nullptr;
	GXAVLTreeNode* currentNode = root;

	while ( currentNode )
	{
		GXInt compareResult = Compare ( node, *currentNode );

		if ( compareResult < 0 )
		{
			*parent = currentNode;
			side = eGXAVLTreeSide::Left;
			currentNode = currentNode->left;
		}
		else if ( compareResult > 0 )
		{
			*parent = currentNode;
			side = eGXAVLTreeSide::Right;
			currentNode = currentNode->right;
		}
		else
		{
			*oldNode = currentNode;
			return;
		}
	}

	*parent = nullptr;
	side = eGXAVLTreeSide::Unknown;
	*oldNode = nullptr;
}

GXUInt GXAVLTree::GetHeight ( GXAVLTreeNode* node ) const
{
	return node ? node->height : 0;
}

GXInt GXAVLTree::GetBalance ( GXAVLTreeNode* node )
{
	return static_cast<GXInt> ( GetHeight ( node->right ) ) - static_cast<GXInt> ( GetHeight ( node->left ) );
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

GXAVLTreeNode* GXAVLTree::Insert ( GXAVLTreeNode* node, GXAVLTreeNode* currentRoot )
{
	if ( !currentRoot ) return node;

	if ( Compare ( *node, *currentRoot ) < 0 )
		currentRoot->left = Insert ( node, currentRoot->left );
	else
		currentRoot->right = Insert ( node, currentRoot->right );

	return Balance ( currentRoot );
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

	GXInt difference = GetBalance ( me );

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
