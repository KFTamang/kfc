#include "9cc.h"


int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "wrong number of arguments\n");
  return 1;
  }


  // tokenize and parse
  user_input = argv[1];
  token = tokenize(user_input);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // climbing down the tree and generate code
  gen(node);
  
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
