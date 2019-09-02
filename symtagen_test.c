#include "kfc.h"

int main(){

  char* test_str = "main(){return 0;}";
  locals = NULL;
  g_label_num = 0;
  token = tokenize(test_str);
  program();
  int i = 0;
  while(code[i] != NULL){
    symtabgen(code[i], code[i]);
    ++i;
  }
  
  return 0;
}
