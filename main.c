#include "kfc.h"

Scope* g_global_scope;
Scope* g_current_scope;

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "wrong number of arguments\n");
  return 1;
  }
  // initialize scope
  g_global_scope = gen_new_scope(NULL, GLOBAL);
  g_current_scope = g_global_scope;

  // initialize label number
  g_label_num = 0;
  
  // tokenize and parse
  user_input = argv[1];
  token = tokenize(user_input);
  program();

  printf(".intel_syntax noprefix\n");

  // generate global variable space
  printf(".bss\n");
  gen_global_var(g_global_scope);

  // print all the string literals in rodata section
  printf("  .text\n");
  printf("  .section .rodata\n");
  print_all_strltrs();

  // climbing down the tree and generate code
  printf(".text\n");
  int i = 0;
  while(code[i] != NULL){
    if(i > 0){
      // take out evaluation result of equation
      // printf("  pop rax\n");
    }
    gen(code[i]);
    //    tree_print(code[i], 0);
    ++i;
  }

  return 0;
}
