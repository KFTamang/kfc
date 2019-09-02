#include "kfc.h"

void compile(char* input_code);

int main(){
  compile("main(){return;}");
  compile("main(){a=1;b=3; if(a>0){a = a + 2;} return a+b;}");
  return 0;
}

void compile(char* input_code){

  // initialize local variable chain
  locals = NULL;

  // initialize label number
  g_label_num = 0;

  // tokenize and parse
  //  user_input = "main(){return 0;}";
  user_input = "main(){return 0;}";
  token = tokenize(user_input);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");

  int i = 0;
  while(code[i] != NULL){
	symtabgen(code[i],code[i]);
    ++i;
  }

  // climbing down the tree and generate code
  i = 0;
  while(code[i] != NULL){
    gen(code[i]);
    ++i;

    // take out evaluation result of equation
    printf("  pop rax\n");
  }

  // epilogue
  // the last evaluation result in rax is the return value
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return;
}
