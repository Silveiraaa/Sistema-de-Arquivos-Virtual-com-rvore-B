#include "filesystem.h"

BTreeNode* create_btree_node(int leaf) {
    BTreeNode* node = malloc(sizeof(BTreeNode));
    node->num_keys = 0;
    node->leaf = leaf;
    for (int i = 0; i < 2 * BTREE_ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

BTree* btree_create() {
    BTree* tree = malloc(sizeof(BTree));
    tree->root = create_btree_node(1); // Inicialmente folha
    return tree;
}

void btree_traverse_node(BTreeNode* node) {
    if (node == NULL) return;

    for (int i = 0; i < node->num_keys; i++) {
        if (!node->leaf) {
            btree_traverse_node(node->children[i]);
        }
        printf("%s (%s)\n", node->keys[i]->name, node->keys[i]->type == FILE_TYPE ? "Arquivo" : "Diretório");
    }

    if (!node->leaf) {
        btree_traverse_node(node->children[node->num_keys]);
    }
}

void btree_traverse(BTree* tree) {
    if (tree->root != NULL) {
        btree_traverse_node(tree->root);
    }
}

TreeNode* btree_search_node(BTreeNode* node, const char* name) {
    int i = 0;
    while (i < node->num_keys && strcmp(name, node->keys[i]->name) > 0) {
        i++;
    }

    if (i < node->num_keys && strcmp(name, node->keys[i]->name) == 0) {
        return node->keys[i];
    }

    if (node->leaf) {
        return NULL;
    }

    return btree_search_node(node->children[i], name);
}

TreeNode* btree_search(BTree* tree, const char* name) {
    if (tree->root == NULL) return NULL;
    return btree_search_node(tree->root, name);
}

void btree_split_child(BTreeNode* parent, int index, BTreeNode* child) {
    BTreeNode* new_node = create_btree_node(child->leaf);
    new_node->num_keys = BTREE_ORDER - 1;

    for (int j = 0; j < BTREE_ORDER - 1; j++) {
        new_node->keys[j] = child->keys[j + BTREE_ORDER];
    }

    if (!child->leaf) {
        for (int j = 0; j < BTREE_ORDER; j++) {
            new_node->children[j] = child->children[j + BTREE_ORDER];
        }
    }

    for (int j = parent->num_keys; j >= index + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = new_node;

    for (int j = parent->num_keys - 1; j >= index; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[index] = child->keys[BTREE_ORDER - 1];
    parent->num_keys++;
    child->num_keys = BTREE_ORDER - 1;
}

void btree_insert_non_full(BTreeNode* node, TreeNode* new_node) {
    int i = node->num_keys - 1;

    if (node->leaf) {
        while (i >= 0 && strcmp(new_node->name, node->keys[i]->name) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = new_node;
        node->num_keys++;
    } else {
        while (i >= 0 && strcmp(new_node->name, node->keys[i]->name) < 0) {
            i--;
        }
        i++;
        if (node->children[i]->num_keys == 2 * BTREE_ORDER - 1) {
            btree_split_child(node, i, node->children[i]);
            if (strcmp(new_node->name, node->keys[i]->name) > 0) {
                i++;
            }
        }
        btree_insert_non_full(node->children[i], new_node);
    }
}

void btree_insert(BTree* tree, TreeNode* new_node) {
    BTreeNode* root = tree->root;
    if (root->num_keys == 2 * BTREE_ORDER - 1) {
        BTreeNode* new_root = create_btree_node(0);
        new_root->children[0] = root;
        tree->root = new_root;
        btree_split_child(new_root, 0, root);
        btree_insert_non_full(new_root, new_node);
    } else {
        btree_insert_non_full(root, new_node);
    }
}

void btree_delete_node(BTreeNode* node, const char* name) {
    if (node == NULL) return;

    int i;
    for (i = 0; i < node->num_keys; i++) {
        if (strcmp(node->keys[i]->name, name) == 0) {
            break;
        }
    }

    if (i == node->num_keys) {
        // Se não encontrou aqui, procura nos filhos (se tiver)
        if (!node->leaf) {
            for (int j = 0; j <= node->num_keys; j++) {
                btree_delete_node(node->children[j], name);
            }
        }
        return;
    }

    // Achou: remove deslocando os elementos
    for (int j = i; j < node->num_keys - 1; j++) {
        node->keys[j] = node->keys[j + 1];
    }
    node->num_keys--;

    printf("Item '%s' deletado da árvore B.\n", name);
}

void btree_delete(BTree* tree, const char* name) {
    if (tree->root != NULL) {
        btree_delete_node(tree->root, name);
    }
}