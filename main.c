#include "kfc.h"

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "wrong number of arguments\n");
  return 1;
  }

  // initialize constants
  VAR_NAME_SIZE = 512;
  MAX_ARG_NUM = 6;
  ARG_REG[0] = "rdi";
  ARG_REG[1] = "rsi";
  ARG_REG[2] = "rdx";
  ARG_REG[3] = "rcx";
  ARG_REG[4] = "r8";
  ARG_REG[5] = "r9";
  MAX_SYMBOL_NUM = 65535; // 16bit for now


  // initialize scope
  initialize_global_scope();
  initialize_current_scope();

  // initialize label number
  g_label_num = 0;
  
  // tokenize and parse
  user_input = read_file(argv[1]);
  token = tokenize(user_input);
  program();

  printf(".intel_syntax noprefix\n");

  // generate global variable space
  printf(".bss\n");
  gen_global_var(get_global_scope());

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
