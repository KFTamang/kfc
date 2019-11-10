#include <stdio.h>
#include <stdlib.h>

void hoge(){printf("OK\n");}

int foo(int x){return x;};
int bar(int x, int y){return x+y;}
int bar3(int x, int y, int z){return x+y+z;}
int bar4(int x, int y, int z, int a){return x+y+z+a;}
int bar5(int x, int y, int z, int a, int b){return x+y+z+a+b;}
int bar6(int x, int y, int z, int a, int b, int c){return x+y+z+a+b+c;}

int alloc4(long** p, int a, int b, int c, int d){
  long* arr = (long*)malloc(4*sizeof(long));
  if(!arr){
    return 1;
  }
  arr[0] = (long)a;
  arr[1] = (long)b;
  arr[2] = (long)c;
  arr[3] = (long)d;
  *p = arr;
  return 0;
}
