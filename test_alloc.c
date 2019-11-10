#include <stdio.h>
int alloc4(long** p, int a, int b, int c, int d);

int main(){

  long* p;
  alloc4(&p, 5,6,7,8);
  printf("%ld %ld %ld %ld\n",p[0], p[1], p[2], p[3]);
  return 0;
}
