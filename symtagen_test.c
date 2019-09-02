#include "kfc.h"

void print_sym_table(){
  printf("kind, name, size, addr, scope name\n");
	 
  for(int i=0;i<MAX_SYMBOL_NUM;i++){
    if(sym_table[i] != NULL){
      printf("%d, %s, %d, %d, %s\n", sym_table[i]->kind, sym_table[i]->name,
      	     sym_table[i]->size,  sym_table[i]->addr, sym_table[i]->scope->name);
    }
  }
}

int main(){

  char* test_str = "main(){\na=0;\nb=0;\nreturn 0;\n}";
  locals = NULL;
  g_label_num = 0;
  token = tokenize(test_str);
  program();
  int i = 0;
  while(code[i] != NULL){
    symtabgen(code[i], code[i]);
    ++i;
  }

  print_sym_table();
  
  return 0;
}
