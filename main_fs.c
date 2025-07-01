#include "filesystem.h"

int main() {
  Directory *root = get_root_directory();
  Directory *current = root;

  // Estrutura inicial
  TreeNode *dirSO = create_directory("SO");
  TreeNode *dirTEST = create_directory("TEST");
  dirSO->data.directory->parent = root;
  dirTEST->data.directory->parent = root;
  btree_insert(root->tree, dirSO);
  btree_insert(root->tree, dirTEST);
  TreeNode *file1 = create_txt_file("arquivo1.txt", "Arquivo de teste de SO.");
  btree_insert(dirSO->data.directory->tree, file1);

  char command[256];
  while (1) {
    printf("\n> ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = 0;

    if (strcmp(command, "ls") == 0) {
      list_directory_contents(current);
    } else if (strncmp(command, "cd ", 3) == 0) {
      char *dir_name = command + 3;
      change_directory(&current, dir_name);
    } else if (strncmp(command, "rm ", 3) == 0) {
      char *name = command + 3;
      delete_txt_file(current->tree, name);
    } else if (strncmp(command, "rmdir ", 6) == 0) {
      char *name = command + 6;
      delete_directory(current->tree, name);
    } else if (strcmp(command, "touch") == 0) {
      create_txt_file_interactive(current->tree);
    } else if (strcmp(command, "mkdir") == 0) {
      create_directory_interactive(current->tree, current);
    } else if (strcmp(command, "exit") == 0) {
      export_to_fs_img(root);
      break;
    } else {
      printf("Comando inválido. Comandos: ls, cd <dir>, rm <arquivo>, rmdir "
             "<dir>, touch, mkdir, exit\n");
    }
  }

  return 0;
}
