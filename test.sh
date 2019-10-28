#!/bin/bash

try(){
    expected="$1"
    input="$2"
    echo $input
    ./kfc "$input" > tmp.s
    gcc -o tmp tmp.s hoge.o foo.o
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

src="main(){
  int a;
  a = 5;
  return a;
}"
try "5" "$src"

src="main(){
    int a;
    a = 5;
    int b; 
    b = 9;
    a = a + 1;
    return a+b;
  }"
try "15" "$src"

src="
return_9(){
return 9;
}

main(){
  return return_9();
}"
try "9" "$src"

src="
return_7(){
return 7;
}

main(){
  int a;
  a = return_7();
  return a;
}"
try "7" "$src"

src="
return_9(){
return 9;
}

main(){
  int a;
  a = 9;
  return a;
}"
try "9" "$src"


src="
return_5(){
int a;
a = 5;
return a;
}

main(){
  return return_5();
}"
try "5" "$src"

src="
return_3(){
int a;
a = 3;
return a;
}

main(){
  int b;
  int c;
  int d;
  b = return_3();
  return b;
}"
try "3" "$src"

src="
return_3(){
int a;
a = 3;
return a;
}

main(){
  int b;
  b = return_3();
  return b;
}"
try "3" "$src"


src="
return_7(int x){
  return 7;
}

 main(){
   return 7;
 }"
try "7" "$src"

src="
return_7(int x){
  return 7;
}

 main(){
   return return_7(0);
 }"
try "7" "$src"

src="
return_x(int x){
  return x;
}

main(){
  return return_x(9);
}"
try "9" "$src"

src="
return_x(int x){
  return x;
}

main(){
  int a;
  a = 9;
  return return_x(a+7);
}"
try "16" "$src"

# src8="
# add_x_y(x, y){
#   return x+y;
# }

# main(){
#   return add_x_y(6,10);
# }"
# try "16" "$src8"

# src9="
# main(){
#   if(5<3){
#     return 7;
#   }else{
#     return 2;
#   }
# }
# "
# try "2" "$src9"

# src10="
# add(x,y){
#   return x+y;
# }
# main(){
#   if(add(5,3)==8){
#     return 7;
#   }else{
#     return 2;
#   }
# }
# "
# try "7" "$src10"

# fibonacci="
# fib(x){
#   if(x<3){
#     return 1;
#   }else{
#     return fib(x-1) + fib(x-2);
#   }
# }

# main(){
#   if( fib(1) == 1 ){
#     return 3;
#   }else{
#     return 5;
#   }
# }
# "
# try "3" "$fibonacci"

# fibonacci="
# fib(x){
#   if(x<3){
#     return 1;
#   }else{
#     return fib(x-1) + fib(x-2);
#   }
# }

# main(){
#   if( fib(5) == 5 ){
#     return 3;
#   }else{
#     return 5;
#   }
# }
# "
# try "3" "$fibonacci"

# fibonacci="
# fib(x){
#   if(x<3){
#     return 1;
#   }else{
#     return fib(x-1) + fib(x-2);
#   }
# }

# main(){
#   if( fib(13) == 233 ){
#     return 5;
#   }else{
#     return 3;
#   }
# }
# "
# try "5" "$fibonacci"

# src="test(){
#   x = 3;
#   y = &x;
#   return *y;
# }

# main(){
#   return test();
# }
# "
# try "3" "$src"

# src="test(){
#   x = 3;
#   y = 5;
#   z = &x - 8;
#   return *z;
# }

# main(){
#   return test();
# }
# "
# try "5" "$src"

echo "test passed"


