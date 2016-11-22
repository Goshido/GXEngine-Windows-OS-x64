//version 1.1

#ifndef GX_AVL_TREE
#define GX_AVL_TREE


#include <GXCommon/GXTypes.h>


class GXAVLTreeNode
{
	public:
		GXAVLTreeNode*	left;
		GXAVLTreeNode*	right;
		GXUInt			height;

	public:
		GXAVLTreeNode ();
		virtual ~GXAVLTreeNode ();
	
		virtual const GXVoid* GetKey () = 0;
};


typedef GXVoid ( GXCALL* PFNGXAVLTREEITERATORPROC ) ( const GXAVLTreeNode* node, GXVoid* args );
typedef GXInt ( GXCALL* PFNGXAVLTREECOMPAREPROC ) ( const GXVoid* a, const GXVoid* b );


class GXAVLTree
{
	protected:
		GXAVLTreeNode*				root;
		GXUInt						totalNodes;

	private:
		PFNGXAVLTREECOMPAREPROC		Compare;
		GXBool						isAutoClean;

	public:
		GXUInt GetTotalNodes ();

	protected:
		GXAVLTree ( PFNGXAVLTREECOMPAREPROC compareFunc, GXBool isAutoClean = GX_TRUE );
		virtual ~GXAVLTree ();

		const GXAVLTreeNode* FindByKey ( const GXVoid* key );
		GXVoid Add ( GXAVLTreeNode* node );
		
		GXVoid DoPrefix ( const GXAVLTreeNode* root, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args );
		GXVoid DoInfix ( const GXAVLTreeNode* root, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args );
		GXVoid DoPostfix ( const GXAVLTreeNode* root, PFNGXAVLTREEITERATORPROC iterator, GXVoid* args );

	private:
		GXUInt GetHeight ( GXAVLTreeNode* node );
		GXInt GetBalance ( GXAVLTreeNode* node );

		GXAVLTreeNode* LeftRotate ( GXAVLTreeNode* father );
		GXAVLTreeNode* RightRotate ( GXAVLTreeNode* father );

		GXAVLTreeNode* Insert ( GXAVLTreeNode* node, GXAVLTreeNode* root );
		GXVoid FixHeight ( GXAVLTreeNode* node );
		GXAVLTreeNode* Balance ( GXAVLTreeNode* me );
		
		GXVoid DeleteTree ( GXAVLTreeNode* node );
};


#endif //GX_AVL_TREE