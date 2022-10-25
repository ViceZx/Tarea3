#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;

struct TreeNode{
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap{
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}

TreeNode * createTreeNode(void* key, void * value){
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair= (Pair *)malloc(sizeof(Pair));
    new->pair->key= key;
    new->pair->value= value;
    new->parent= new->left= new->right= NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)){
    TreeMap* new= (TreeMap *) malloc(sizeof(TreeMap));
    new->root= NULL;
    new->current= NULL;
    new->lower_than= lower_than;
    return new;
}

void insertTreeMap(TreeMap *tree, void *key, void *value)
{
    if (tree == NULL) return;

    TreeNode *aux = tree->root;
    TreeNode *parent = NULL;

    while (aux != NULL){
        parent= aux;
        if (tree->lower_than(key, aux->pair->key)) aux = aux->left;
        else if (tree->lower_than(aux->pair->key, key)) aux = aux->right;
        else return;

    }
    TreeNode *new = createTreeNode(key, value);
    new->parent= parent;
    if (parent == NULL) tree->root= new;
    else if (tree->lower_than(key, parent->pair->key)) parent->left= new;
    else parent->right= new;
    tree->current= new;
}

TreeNode * minimum(TreeNode * x){
    TreeNode *aux= x;
    while (aux->left != NULL)
        aux= aux->left;
    return aux;
}

TreeNode * maximum(TreeNode * x){
    TreeNode * aux= x;
    while (aux->right != NULL)
        aux= aux->right;
    return aux;
}

void removeNode(TreeMap * tree, TreeNode* node){
    TreeNode* aux= node;
    if (node->left == NULL && node->right == NULL){
        if (node->parent->left == node) node->parent->left= NULL;
        if (node->parent->right == node) node->parent->right= NULL;
        free(node);
        return;
    }
    if (node->left == NULL && node->right != NULL){
        if (node->parent->left == node){
            aux= node->right;
            node->parent->left= aux;
        }
        if (node == node->parent->right){
            aux= node->right;
            node->parent->right= aux;
        }
        aux->parent= node->parent;
        free(node);
        return;
    }
    if (node->right == NULL && node->left != NULL){
        if (node->parent->left == node){
            aux= node->left;
            node->parent->left= aux;
        }
        if (node == node->parent->right){
            aux= node->left;
            node->parent->right= aux;
        }
        aux->parent= node->parent;
        free(node);
        return;
    }
    if (node->right != NULL && node->left != NULL){
        aux= minimum(node->right);
        node->pair= aux->pair;
        removeNode(tree, aux);
    }
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;
    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node= tree->current;
    removeNode(tree, node);
}

Pair * searchTreeMap(TreeMap * tree, void* key){
    TreeNode* aux= tree->root;
    while (aux != NULL){
        if (tree->lower_than(key, aux->pair->key) == 1) aux = aux->left;
        else if (tree->lower_than(aux->pair->key, key) == 1) aux = aux->right;
        else{
          tree->current= aux;
          return aux->pair;
        }
    }
    return NULL;
}

Pair * upperBound(TreeMap * tree, void* key){
    TreeNode* uppNode= tree->root;
    TreeNode* auppNode= tree->root;
    while (uppNode != NULL){
        if (tree->lower_than(key, uppNode->pair->key) == 1){
            auppNode= uppNode;
            uppNode= uppNode->left;
        }
        else if (tree->lower_than(uppNode->pair->key, key) == 1){
            if (tree->lower_than(auppNode->pair->key, uppNode->pair->key) == 1) auppNode= uppNode;
            uppNode= uppNode->right;
        }
        else return uppNode->pair;
    }
    if (tree->lower_than(auppNode->pair->key, key)) return NULL;
    return auppNode->pair;
}

Pair * firstTreeMap(TreeMap * tree){
    TreeNode * aux= minimum(tree->root);
    Pair * min= aux->pair;
    return min;
}

Pair * lastTreeMap(TreeMap * tree){
    TreeNode * aux= maximum(tree->root);
    Pair * min= aux->pair;
    return min;
}

Pair * nextTreeMap(TreeMap * tree){
    TreeNode * aux= tree->current;
    if (aux->right != NULL){
        aux= minimum(tree->current->right);
        tree->current = aux;
        return aux->pair;
    }
    else{
        TreeNode * ant= tree->current->parent;
        while (ant != NULL){
            if (tree->lower_than(aux->pair->key, ant->pair->key) == 1){
                tree->current= ant;
                return ant->pair;
            }
            ant= ant->parent;
        }
    }
    return NULL;
}

Pair * prevTreeMap(TreeMap * tree){
    TreeNode * aux = tree->current;
    if (aux->left != NULL){
        aux = maximum(tree->current->left);
        tree->current = aux;
        return aux->pair;
    }
    else{
        TreeNode * ant= tree->current->parent;
        while (ant != NULL){
            if (tree->lower_than(aux->pair->key, ant->pair->key) == 0){
                tree->current = ant;
                return ant->pair;
            }
            ant= ant->parent;
        }
    }
    return NULL;
}