#include "filesystem.h"

TreeNode* create_txt_file(const char* name, const char* content) {
    File* file = malloc(sizeof(File));
    file->name = strdup(name);
    file->content = strdup(content);
    file->size = strlen(content);

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = FILE_TYPE;
    node->data.file = file;
    return node;
}

TreeNode* create_directory(const char* name) {
    Directory* dir = malloc(sizeof(Directory));
    dir->tree = btree_create();
    dir->parent = NULL; 

    TreeNode* node = malloc(sizeof(TreeNode));
    node->name = strdup(name);
    node->type = DIRECTORY_TYPE;
    node->data.directory = dir;
    return node;
}
void delete_txt_file(BTree* tree, const char* name) {
    TreeNode* target = btree_search(tree, name);
    if (target && target->type == FILE_TYPE) {
        File* file_data = target->data.file;

        btree_delete(tree, name);

        free(file_data->content);
        free(file_data->name);
        free(file_data);
        free(target->name);
        free(target);

        printf("Arquivo '%s' deletado.\n", name);
    } else {
        printf("Arquivo '%s' não encontrado ou não é um arquivo.\n", name);
    }
}



void delete_directory(BTree* tree, const char* name) {
    TreeNode* node = btree_search(tree, name);
    if (!node || node->type != DIRECTORY_TYPE) {
        printf("Diretório '%s' não encontrado.\n", name);
        return;
    }

    if (node->data.directory->tree->root != NULL && node->data.directory->tree->root->num_keys > 0) {
        printf("Erro: Diretório '%s' não está vazio.\n", name);
        return;
    }
    btree_delete(tree, name);

    free(node->data.directory->tree->root);
    free(node->data.directory->tree);
    free(node->data.directory);
    free(node->name);
    free(node);

    printf("Diretório '%s' deletado.\n", name);
}




Directory* get_root_directory() {
    Directory* root = malloc(sizeof(Directory));
    root->tree = btree_create();
    return root;
}

void change_directory(Directory** current, const char* path) {
    if (strcmp(path, "..") == 0) {
        if ((*current)->parent != NULL) {
            *current = (*current)->parent;
            printf("Voltou para o diretório pai.\n");
        } else {
            printf("Já está na raiz.\n");
        }
        return;
    }

    TreeNode* target = btree_search((*current)->tree, path);

    if (target && target->type == DIRECTORY_TYPE) {
        *current = target->data.directory;
        printf("Mudou para o diretório: %s\n", path);
    } else {
        printf("Diretório '%s' não encontrado.\n", path);
    }
}

void list_directory_contents(Directory* dir) {
    printf("Conteúdo do diretório:\n");
    if (dir && dir->tree) {
        btree_traverse(dir->tree);
    } else {
        printf("(Diretório vazio ou inválido)\n");
    }
}

void create_txt_file_interactive(BTree* tree) {
    char name[256];
    printf("Nome do arquivo: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    if (btree_search(tree, name)) {
        printf("Erro: já existe um arquivo ou diretório com esse nome.\n");
        return;
    }

    char conteudo[1048576]; // 1MB
    printf("Digite o conteúdo do arquivo (fim com ENTER):\n");
    fgets(conteudo, sizeof(conteudo), stdin);
    conteudo[strcspn(conteudo, "\n")] = 0;

    if (strlen(conteudo) > 1048576) {
        printf("Erro: conteúdo excede 1MB.\n");
        return;
    }

    TreeNode* newFile = create_txt_file(name, conteudo);
    btree_insert(tree, newFile);
    printf("Arquivo '%s' criado.\n", name);
}

void create_directory_interactive(BTree* tree, Directory* parent) {
    char name[256];
    printf("Nome do diretório: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    if (btree_search(tree, name)) {
        printf("Erro: já existe um arquivo ou diretório com esse nome.\n");
        return;
    }

    TreeNode* newDir = create_directory(name);
    newDir->data.directory->parent = parent;
    btree_insert(tree, newDir);

    printf("Diretório '%s' criado.\n", name);
}


void export_directory(FILE* img, Directory* dir, int depth) {
    BTreeNode* node = dir->tree->root;
    if (!node) return;
    for (int i = 0; i < node->num_keys; i++) {
        TreeNode* entry = node->keys[i];
        for (int d = 0; d < depth; d++) {
            fprintf(img, "│   ");
        }

        if (entry->type == FILE_TYPE) {
            fprintf(img, "├── %s: %s\n", entry->name, entry->data.file->content);
        } else {
            fprintf(img, "├── %s/\n", entry->name);
            export_directory(img, entry->data.directory, depth + 1);
        }
    }
}

void export_to_fs_img(Directory* root) {
    FILE* img = fopen("fs.img", "w");
    if (!img) {
        perror("Erro ao criar fs.img");
        return;
    }

    fprintf(img, "ROOT\n");
    export_directory(img, root, 1);

    fclose(img);
    printf("Sistema de arquivos salvo em fs.img\n");
}
