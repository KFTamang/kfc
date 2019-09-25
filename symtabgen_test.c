#include "kfc.h"

void print_sym_table(){
  printf("kind, name, size, addr, scope name\n");
	 
  for(int i=0;i<MAX_SYMBOL_NUM;i++){
    if(sym_table[i] != NULL){
      printf("%d, %s, %d, %d, %s\n", sym_table[i]->kind, sym_table[i]->name,
      	     sym_table[i]->size,  sym_table[i]->addr, sym_table[i]->scope);
      printf("address: %d\n",get_lvar_address(sym_table[i]->scope, sym_table[i]->name));
    }
  }
}

int main(){

  //  char* test_str = "main(){a=1;b=3; a=2; return a+b;}";
  //  char* test_str = "main(){a=1;b=3; if(a>0){a = a + 2;} return a+b;}";
  //  char* test_str = "main(){a=1;b=3; for(i=0;i<10;i=i+1){a = a + 2;} return a+b;}";
  //  char* test_str = "main(){a=1;b=3; return a+b;}";
  char* test_str = "fib(x){ \
  if(x<3){ \
    return 1; \
  }else{ \
    return fib(x-1) + fib(x-2); \
  } \
} \
 \
main(){ \
  return fib(3); \
}";
  locals = NULL;
  g_label_num = 0;
  token = tokenize(test_str);
  program();
  int i = 0;
  printf("symtab kind SY_LVAR:%d SY_FUNC_DEF:%d SY_GVAR:%d\n",SY_LVAR, SY_FUNC, SY_GVAR);
  while(code[i] != NULL){
    symtabgen(code[i], code[i]);
    ++i;
  }

  print_sym_table();
  
  return 0;
}
