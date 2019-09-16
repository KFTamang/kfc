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

echo "test passed"


