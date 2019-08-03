// vesrion 1.10

#include <GXCommon/GXAVLTree.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXLogger.h>


GXAVLTreeNode::GXAVLTreeNode ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXAVLTreeNode" )
{
    _left = _right = nullptr;
    _height = 1u;
}

GXAVLTreeNode::~GXAVLTreeNode ()
{
    // NOTHING
}

//----------------------------------------------------------------------------------------------

GXUInt GXAVLTree::GetTotalNodes () const
{
    return _totalNodes;
}

GXAVLTree::GXAVLTree ( GXAVLTreeComparator comparator, GXBool doesAutoClean )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXAVLTree" )
    _root ( nullptr ),
    _totalNodes ( 0u ),
    _comparator ( comparator ),
    _isAutoClean ( doesAutoClean )
{
    // NOTHING
}

GXAVLTree::~GXAVLTree ()
{
    if ( !_isAutoClean ) return;

    DeleteTree ( _root );
}

const GXAVLTreeNode* GXAVLTree::Find ( const GXAVLTreeNode &node ) const
{
    GXAVLTreeNode* p = _root;

    while ( p )
    {
        switch ( _comparator ( node, *p ) )
        {
            case eGXCompareResult::Less:
                p = p->_left;
            break;

            case eGXCompareResult::Equal:
            return p;

            case eGXCompareResult::Greater:
                p = p->_right;
            break;
        }
    }

    return nullptr;
}

GXVoid GXAVLTree::Add ( GXAVLTreeNode &node )
{
    GXAVLTreeNode* parentNode = nullptr;
    eGXAVLTreeSide side = eGXAVLTreeSide::Unknown;
    GXAVLTreeNode* oldNode;

    FindInternal ( &oldNode, &parentNode, side, node );

    if ( !oldNode )
    {
        _root = Insert ( &node, _root );
        ++_totalNodes;
        return;
    }

    if ( !_isAutoClean )
    {
        GXLogA ( "GXAVLTree::Add::Error - Обнаружен элемент с идентичным ключом. Не могу выполнить обмен элементов, так как требуется удаление старого элемента, а режим авточистки запрещён.\n" );
        return;
    }

    if ( !parentNode )
    {
        _root = &node;
    }
    else
    {
        switch ( side )
        {
            case eGXAVLTreeSide::Left:
                parentNode->_left = &node;
            break;

            case eGXAVLTreeSide::Right:
                parentNode->_right = &node;
            break;

            case eGXAVLTreeSide::Unknown:
                GXLogA ( "GXAVLTree::Add::Error - Как код попал в ветку eGXAVLTreeSide::Unknown?\n" );
            break;

            default:
                // NOTHING
            break;
        }
    }

    node._left = oldNode->_left;
    node._right = oldNode->_right;
    node._height = oldNode->_height;

    delete oldNode;
}

GXVoid GXAVLTree::DoPrefix ( const GXAVLTreeNode* currentRoot, GXAVLTreeIterator iterator, GXVoid* args ) const
{
    if ( !currentRoot ) return;

    iterator ( *currentRoot, args );
    DoPrefix ( currentRoot->_left, iterator, args );
    DoPrefix ( currentRoot->_right, iterator, args );
}

GXVoid GXAVLTree::DoInfix ( const GXAVLTreeNode* currentRoot, GXAVLTreeIterator iterator, GXVoid* args ) const
{
    if ( !currentRoot ) return;

    DoInfix ( currentRoot->_left, iterator, args );
    iterator ( *currentRoot, args );
    DoInfix ( currentRoot->_right, iterator, args );
}

GXVoid GXAVLTree::DoPostfix ( const GXAVLTreeNode* currentRoot, GXAVLTreeIterator iterator, GXVoid* args ) const
{
    if ( !currentRoot ) return;

    DoPostfix ( currentRoot->_left, iterator, args );
    DoPostfix ( currentRoot->_right, iterator, args );
    iterator ( *currentRoot, args );
}

GXVoid GXAVLTree::FindInternal ( GXAVLTreeNode** oldNode, GXAVLTreeNode** parent, eGXAVLTreeSide &side, const GXAVLTreeNode &node )
{
    *parent = nullptr;
    GXAVLTreeNode* currentNode = _root;

    while ( currentNode )
    {
        switch ( _comparator ( node, *currentNode ) )
        {
            case eGXCompareResult::Less:
                *parent = currentNode;
                side = eGXAVLTreeSide::Left;
                currentNode = currentNode->_left;
            break;

            case eGXCompareResult::Equal:
                *oldNode = currentNode;
            return;

            case eGXCompareResult::Greater:
                *parent = currentNode;
                side = eGXAVLTreeSide::Right;
                currentNode = currentNode->_right;
            break;
        }
    }

    *parent = nullptr;
    side = eGXAVLTreeSide::Unknown;
    *oldNode = nullptr;
}

GXUInt GXAVLTree::GetHeight ( GXAVLTreeNode* node ) const
{
    return node ? node->_height : 0u;
}

GXInt GXAVLTree::GetBalance ( GXAVLTreeNode* node )
{
    return static_cast<GXInt> ( GetHeight ( node->_right ) ) - static_cast<GXInt> ( GetHeight ( node->_left ) );
}

GXAVLTreeNode* GXAVLTree::LeftRotate ( GXAVLTreeNode* father )
{
    GXAVLTreeNode* me = father->_right;

    father->_right = me->_left;
    me->_left = father;

    FixHeight ( father );
    FixHeight ( me );

    return me;
}

GXAVLTreeNode* GXAVLTree::RightRotate ( GXAVLTreeNode* father )
{
    GXAVLTreeNode* me = father->_left;

    father->_left = me->_right;
    me->_right = father;

    FixHeight ( father );
    FixHeight ( me );

    return me;
}

GXAVLTreeNode* GXAVLTree::Insert ( GXAVLTreeNode* node, GXAVLTreeNode* currentRoot )
{
    if ( !currentRoot ) return node;

    if ( _comparator ( *node, *currentRoot ) == eGXCompareResult::Less )
        currentRoot->_left = Insert ( node, currentRoot->_left );
    else
        currentRoot->_right = Insert ( node, currentRoot->_right );

    return Balance ( currentRoot );
}

GXVoid GXAVLTree::FixHeight ( GXAVLTreeNode* node )
{
    GXUInt lh = GetHeight ( node->_left );
    GXUInt rh = GetHeight ( node->_right );

    node->_height = 1u + ( lh > rh ? lh : rh );
}

GXAVLTreeNode* GXAVLTree::Balance ( GXAVLTreeNode* me )
{
    FixHeight ( me );

    GXInt difference = GetBalance ( me );

    if ( difference == 2 )
    {
        GXAVLTreeNode* rightSon = me->_right;

        if ( GetBalance ( rightSon ) < 0 )
            me->_right = RightRotate ( rightSon );

        return LeftRotate ( me );
    }

    if ( difference == -2 )
    {
        GXAVLTreeNode* leftSon = me->_left;

        if ( GetBalance ( leftSon ) > 0 )
            me->_left = LeftRotate ( leftSon );

        return RightRotate ( me );
    }

    return me;
}

GXVoid GXAVLTree::DeleteTree ( GXAVLTreeNode* node )
{
    if ( !node ) return;

    DeleteTree ( node->_left );
    DeleteTree ( node->_right );
    delete node;
}
