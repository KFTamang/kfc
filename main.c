#include "kfc.h"


int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "wrong number of arguments\n");
  return 1;
  }
  // initialize local variable chain
  locals = NULL;

  // initialize label number
  g_label_num = 0;
  
  // tokenize and parse
  user_input = argv[1];
  token = tokenize(user_input);
  program();
  // generate symbol table
  int i = 0;
  while(code[i] != NULL){
    symtabgen(code[i],code[i]);
    ++i;
  }
  

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");

  // climbing down the tree and generate code
  i = 0;
  while(code[i] != NULL){
    if(i > 0){
      // take out evaluation result of equation
      printf("  pop rax\n");
    }
    gen(code[i]);
    //    tree_print(code[i], 0);
    ++i;
  }

  return 0;
}
