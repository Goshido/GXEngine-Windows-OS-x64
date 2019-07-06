// version 1.8

#ifndef GX_AVL_TREE
#define GX_AVL_TREE


#include <GXCommon/GXMemory.h>


class GXAVLTreeNode : public GXMemoryInspector
{
    public:
        GXAVLTreeNode*      _left;
        GXAVLTreeNode*      _right;
        GXUInt              _height;

    public:
        GXAVLTreeNode ();
        ~GXAVLTreeNode () override;
};


typedef GXVoid ( GXCALL* PFNGXAVLTREEITERATORPROC ) ( const GXAVLTreeNode &node, GXVoid* args );
typedef GXInt ( GXCALL* PFNGXAVLTREECOMPAREPROC ) ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );


enum class eGXAVLTreeSide : GXUByte
{
    Left,
    Right,
    Unknown
};


class GXAVLTree
{
    protected:
        GXAVLTreeNode*              _root;
        GXUInt                      _totalNodes;

    private:
        PFNGXAVLTREECOMPAREPROC     _comparator;
        GXBool                      _isAutoClean;

    public:
        GXUInt GetTotalNodes () const;

    protected:
        explicit GXAVLTree ( PFNGXAVLTREECOMPAREPROC comparator, GXBool doesAutoClean );
        virtual ~GXAVLTree ();

        const GXAVLTreeNode* Find ( const GXAVLTreeNode &node ) const;
        GXVoid Add ( GXAVLTreeNode &node );
        
        GXVoid DoPrefix ( const GXAVLTreeNode* currentRoot, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const;
        GXVoid DoInfix ( const GXAVLTreeNode* currentRoot, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const;
        GXVoid DoPostfix ( const GXAVLTreeNode* currentRoot, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args ) const;

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
};


#endif // GX_AVL_TREE
