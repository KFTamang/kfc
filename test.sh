#!/bin/bash

try(){
    expected="$1"
    input="$2"
    echo $input
    ./kfc "$input" > tmp.s
    gcc -o tmp test_helper.o tmp.s # hoge.o foo.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
	echo "$input => $actual"
	echo "OK"
	echo ""
    else
	echo "$input => $expected expected, but got $actual"
	cat tmp.s
	exit 1
    fi
}
func_test(){
    expected="$1"
    input="$2"
    gcc -c hoge.c
    ./kfc "$input" > tmp.s
    gcc -o tmp tmp.s hoge.o foo.o
    actual=`./tmp`

    if [ "$actual" = "$expected" ]; then
	echo "$input => $actual"
    else
	echo "$input => $expected expected, but got $actual"
	cat tmp.s
	exit 1
    fi
}

src="int main(){
  int a;
  a = 5;
  return a;
}"
try "5" "$src"

src="int main(){
    int a;
    a = 5;
    int b; 
    b = 9;
    a = a + 1;
    return a+b;
  }"
try "15" "$src"

src="
int return_9(){
return 9;
}

int main(){
  return return_9();
}"
try "9" "$src"

src="
int return_7(){
return 7;
}

int main(){
  int a;
  a = return_7();
  return a;
}"
try "7" "$src"

src="
int return_9(){
return 9;
}

int main(){
  int a;
  a = 9;
  return a;
}"
try "9" "$src"


src="
int return_5(){
int a;
a = 5;
return a;
}

int main(){
  return return_5();
}"
try "5" "$src"

src="
int return_3(){
int a;
a = 3;
return a;
}

int main(){
  int b;
  int c;
  int d;
  b = return_3();
  return b;
}"
try "3" "$src"

src="
int return_3(){
int a;
a = 3;
return a;
}

int main(){
  int b;
  b = return_3();
  return b;
}"
try "3" "$src"


src="
int return_7(int x){
  return 7;
}

int main(){
   return 7;
 }"
try "7" "$src"

src="
int return_7(int x){
  return 7;
}

int main(){
   return return_7(0);
 }"
try "7" "$src"

src="
int return_x(int x){
  return x;
}

int main(){
  return return_x(9);
}"
try "9" "$src"

src="
int return_x(int x){
  return x;
}

int main(){
  int a;
  a = 9;
  return return_x(a+7);
}"
try "16" "$src"

src="
int add_x_y(int x, int y){
  return x+y;
}

int main(){
  return add_x_y(6,10);
}"
try "16" "$src"

src="
int add(int x, int y){
  return x+y;
}
int main(){
  if(add(5,3)==8){
    return 7;
  }else{
    return 2;
  }
}
"
try "7" "$src"

fibonacci="
int fib(int x){
  if(x<3){
    return 1;
  }else{
    return fib(x-1) + fib(x-2);
  }
}

int main(){
  if( fib(1) == 1 ){
    return 3;
  }else{
    return 5;
  }
}
"
try "3" "$fibonacci"

fibonacci="
int fib(int x){
  if(x<3){
    return 1;
  }else{
    return fib(x-1) + fib(x-2);
  }
}

int main(){
  if( fib(5) == 5 ){
    return 3;
  }else{
    return 5;
  }
}
"
try "3" "$fibonacci"

fibonacci="
int fib(int x){
  if(x<3){
    return 1;
  }else{
    return fib(x-1) + fib(x-2);
  }
}

int main(){
  if( fib(13) == 233 ){
    return 5;
  }else{
    return 3;
  }
}
"
try "5" "$fibonacci"

src="
int add_x_y_z(int x, int y, int z){
  return x+y+z;
}

int main(){
  return add_x_y_z(6,10,7);
}"
try "23" "$src"

src="
int add_x_y_z_minus_v(int x, int y, int z, int v){
  return x+y+z-v;
}

int main(){
  return add_x_y_z_minus_v(6,10,7,5);
}"
try "18" "$src"

src="
int add_x_y_z_times_v(int x, int y, int z, int v){
  return (x+y+z)*v;
}

int main(){
  return add_x_y_z_times_v(6,10,4,2);
}"
try "40" "$src"

src="
int return_x(int x){
  return x;
}

int main(){
  if(return_x(-2) == -2){
    return 3;
  }else{
    return 5;
  }
}"
try "3" "$src"

src="int test(){
  int x;
  int y;
  x = 5;
  y = &x;
  return *y;
}

int main(){
  return test();
}
"
try "5" "$src"

src="int test(){
  int x;
  int y;
  int z;
  x = 3;
  y = 7;
  z = &x - 1;
  return *z;
}

int main(){
  return test();
}
"
try "7" "$src"

src="
int main(){
  int x;
  int *y;
  int **z;
  return 11;
}
"
try "11" "$src"

src="
int main(){
  int x;
  int *y;
  y = &x;
  *y = 3;
  if(x == 3){
    return 5;
  }else{
    return 7;
  }
}
"
try "5" "$src"

src="
int main(){
  int a;  a = 3;
  int b;  b = 5;
  int c;  c = 7;
  int *p;
  p = &c;
  return *(p+1);
}
"
try "5" "$src"

src="
int main(){
  int *p;
  alloc4(&p, 1, 3, 5, 7);
  int *q;
  q = p + 3;
  return *q;
}
"
try "7" "$src"


src="
int main(){
  int *p;
  alloc4(&p, 1, 3, 5, 7);
  int *q;
  q = p + 2*1;
  return *q;
}
"
try "5" "$src"

src="
int main(){
  int *p;
  alloc4(&p, 1, 3, 5, 7);
  int *q;
  q = p + 2/2;
  return *q;
}
"
try "3" "$src"

src="
int main(){
  int *p;
  alloc4(&p, 1, 3, 5, 7);
  int *q;
  q = p + 3;
  q = q - 1;
  return *q;
}"
try "5" "$src"

src="
int main(){
  int *p;
  alloc4(&p, 1, 3, 5, 7);
  int *q;
  q = p + 3 - 2;
  return *q;
}"
try "3" "$src"

src="
int main(){
  int x;
  sizeof(x);
  return 0;
}
"
try "0" "$src"

src="
int main(){
  int x;
  return sizeof(x);
}
"
try "8" "$src"

src="
int main(){
  int x;
  return sizeof x;
}
"
try "8" "$src"

src="
int main(){
  int* x;
  return sizeof x;
}
"
try "8" "$src"

src="
int main(){
  int x;
  x = 7;
  return sizeof(x+4);
}
"
try "8" "$src"

src="
int main(){
  int x[10];
  return 7;
}
"
try "7" "$src"

src="
int main(){
  int x[10];
  return sizeof(x);
}
"
try "80" "$src"

src="
int main(){
  int a[2];
  *a = 1;
  *(a + 1) = 2;
  int *p;
  p = a;
  return *p + *(p + 1);
}
"
try "3" "$src"

src="
int main(){
  int a[3];
  *(a + 2) = 4;
  return *(a + 2);
}"
try "4" "$src"

src="
int main(){
  int a[3];
  *a = 1;
  int x;
  x = 2;
  *(a + 2) = 4;
  return *(a + 2);
}"
try "4" "$src"

src="
int main(){
  int x[10];
  *(x + sizeof(x)-1) = 10;
  return   *(x + sizeof(x)-1);
}"
try "10" "$src"

src="
int main(){
  int x[10];
  int y[20];
  *(x + sizeof(x)-1) = 10;
  *(y + sizeof(y)-3) = 9;
  return   *(x + sizeof(x)-1) + *(y + sizeof(y)-3);
}"
try "19" "$src"

src="int main(){ int x[10]; x[9] = 10; return x[9];}"
try "10" "$src"

src="int main(){ int x[10];int y[2]; x[3]=1; y[x[3]]=7;return y[1];}"
try "7" "$src"

#src="int main(){ int x[10];x[3]=9; return 3[x];}"
#try "9" "$src"

echo "test passed"


