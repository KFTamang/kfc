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

src1="main(){
  a = 5;
  b = 9;
  return a+b;
}"
try "14" "$src1"
 src2="main(){
   a = 5;
   b = 9;
   a = a + 1;
   return a+b;
 }"
 
 try "15" "$src2"
 src3="main(){
   a = 5;
   b = 9;
   for(i=0;i<5;i=i+1){
     a = a + 1;
   }
   return a+b;
 }"
try "19" "$src3"
src4="
return_7(){
  return 7;
}

main(){
  return return_7();
}"
try "7" "$src4"

src5="
return_7(x){
  return 7;
}

main(){
  return return_7(0);
}"
try "7" "$src5"

src6="
return_x(x){
  return x;
}

main(){
  return return_x(9);
}"
try "9" "$src6"

src7="
return_x(x){
  return x;
}

main(){
  a = 9;
  return return_x(a+7);
}"
try "16" "$src7"

src8="
add_x_y(x, y){
  return x+y;
}

main(){
  return add_x_y(6,10);
}"
try "16" "$src8"

src9="
main(){
  if(5<3){
    return 7;
  }else{
    return 2;
  }
}
"
try "2" "$src9"

src10="
add(x,y){
  return x+y;
}
main(){
  if(add(5,3)==8){
    return 7;
  }else{
    return 2;
  }
}
"
try "7" "$src10"

fibonacci="
fib(x){
  if(x<3){
    return 1;
  }else{
    return fib(x-1) + fib(x-2);
  }
}

main(){
  if( fib(1) == 1 ){
    return 3;
  }else{
    return 5;
  }
}
"
try "3" "$fibonacci"

fibonacci="
fib(x){
  if(x<3){
    return 1;
  }else{
    return fib(x-1) + fib(x-2);
  }
}

main(){
  if( fib(5) == 5 ){
    return 3;
  }else{
    return 5;
  }
}
"
try "3" "$fibonacci"

fibonacci="
fib(x){
  if(x<3){
    return 1;
  }else{
    return fib(x-1) + fib(x-2);
  }
}

main(){
  if( fib(13) == 233 ){
    return 5;
  }else{
    return 3;
  }
}
"
try "5" "$fibonacci"

src="test(){
  x = 3;
  y = &x;
  return *y;
}

main(){
  return test();
}
"
try "3" "$src"

src="test(){
  x = 3;
  y = 5;
  z = &x - 8;
  return *z;
}

main(){
  return test();
}
"
try "5" "$src"

echo "test passed"


