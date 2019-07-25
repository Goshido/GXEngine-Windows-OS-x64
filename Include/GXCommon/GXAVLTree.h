// version 1.9

#ifndef GX_AVL_TREE
#define GX_AVL_TREE


#include <GXCommon/GXMemory.h>


enum class eGXAVLTreeSide: GXUByte
{
    Left,
    Right,
    Unknown
};


class GXAVLTreeNode : public GXMemoryInspector
{
    public:
        GXAVLTreeNode*      _left;
        GXAVLTreeNode*      _right;
        GXUInt              _height;

    public:
        GXAVLTreeNode ();
        ~GXAVLTreeNode () override;

    private:
        GXAVLTreeNode ( const GXAVLTreeNode &other ) = delete;
        GXAVLTreeNode& operator = ( const GXAVLTreeNode &other ) = delete;
};

typedef GXVoid ( GXCALL* GXAVLTreeIterator ) ( const GXAVLTreeNode &node, GXVoid* args );
typedef eGXCompareResult ( GXCALL* GXAVLTreeComparator ) ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

class GXAVLTree
{
    protected:
        GXAVLTreeNode*          _root;
        GXUInt                  _totalNodes;

    private:
        GXAVLTreeComparator     _comparator;
        GXBool                  _isAutoClean;

    public:
        GXUInt GetTotalNodes () const;

    protected:
        explicit GXAVLTree ( GXAVLTreeComparator comparator, GXBool doesAutoClean );
        virtual ~GXAVLTree ();

        const GXAVLTreeNode* Find ( const GXAVLTreeNode &node ) const;
        GXVoid Add ( GXAVLTreeNode &node );
        
        GXVoid DoPrefix ( const GXAVLTreeNode* currentRoot, GXAVLTreeIterator iterator, GXVoid* args ) const;
        GXVoid DoInfix ( const GXAVLTreeNode* currentRoot, GXAVLTreeIterator iterator, GXVoid* args ) const;
        GXVoid DoPostfix ( const GXAVLTreeNode* currentRoot, GXAVLTreeIterator iterator, GXVoid* args ) const;

    private:
        GXVoid FindInternal ( GXAVLTreeNode** oldNode, GXAVLTreeNode** parent, eGXAVLTreeSide &side, const GXAVLTreeNode &node );

        GXUInt GetHeight ( GXAVLTreeNode* node ) const;
        GXInt GetBalance ( GXAVLTreeNode* node );

        GXAVLTreeNode* LeftRotate ( GXAVLTreeNode* father );
        GXAVLTreeNode* RightRotate ( GXAVLTreeNode* father );

        GXAVLTreeNode* Insert ( GXAVLTreeNode* node, GXAVLTreeNode* currentRoot );
        GXVoid FixHeight ( GXAVLTreeNode* node );
        GXAVLTreeNode* Balance ( GXAVLTreeNode* me );

        GXVoid DeleteTree ( GXAVLTreeNode* node );

        GXAVLTree ( const GXAVLTree &other ) = delete;
        GXAVLTree& operator = ( const GXAVLTree &other ) = delete;
};


#endif // GX_AVL_TREE
