#include "main.h"

void insert(Node **root, int value)
{

    if (*root == NULL)
    {
        initNode(root, value);
        return;
    }

    Node *temp = *root;
    Node *newNode = NULL;
    initNode(&newNode, value);

    while (1)
    {
        if (value < temp->data)
        {
            if (temp->left_Node == NULL)
            {
                temp->left_Node = newNode;
                return;
            }
            temp = temp->left_Node;
        }
        else
        {
            if (temp->right_Node == NULL)
            {
                temp->right_Node = newNode;
                return;
            }
            temp = temp->right_Node;
        }
    }

}

bool contain(Node *root, int value)
{
    if(root == NULL )
        return false;

    while(true) {
        if(value == root->data)
            return true;
        if(value > root->data) {
            if(root->right_Node == NULL ) return false;
            root = root->right_Node;
        } else {
            if(root->left_Node == NULL ) return false;
            root = root->left_Node;
        }
    }
}

void initNode(Node **root, int value)
{
    if (*root != NULL)
        return;
    Node *n = (Node *)malloc(sizeof(Node));
    n->data = value;
    n->left_Node = n->right_Node = NULL;
    *root = n;
}

void inOrderTraverse(Node* root) {

}