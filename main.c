#include "9cc.h"


int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "wrong number of arguments\n");
  return 1;
  }
  // initialize local variable chain
  locals = NULL;

  // initialize label number
  label_num = 0;
  
  // tokenize and parse
  user_input = argv[1];
  token = tokenize(user_input);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue
  // allocate 26 variables
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");
  
  // climbing down the tree and generate code
  int i = 0;
  while(code[i] != NULL){
    gen(code[i]);
    //    tree_print(code[i], 0);
    ++i;

    // take out evaluation result of equation
    printf("  pop rax\n");
  }

  // epilogue
  // the last evaluation result in rax is the return value
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
