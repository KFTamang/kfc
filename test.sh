#!/bin/bash

try(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
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

try 0 "0;"
try 45 "45;"
try 6 '5+10-9;'
try 178 "80 + 100 -2;"
try 198 "100 + 100 - 2;"
try 21  "1 + 2 + 3 + 4 + 5 + 6;"
try 12 "30 - 3 - 4 -5-6;"
#try -16 "2 - 3 - 4 -5-6" # negative number not yet implemented
try 150 "90 - 10 + 120 - 30 + 10 - 20 + 30 - 40;"
try 0 "90-100+1+14-29+24;"
# introduced multiplication
try 2 "1*2;"
try 42 "2*20 + 2;"
try 30 "5*2 + 10*2;"
try 35 "5 + 1* 2*5+10 *2;"
# introduced division
try 6 "30/5;"
try 12 "30/5*2;"
try 3 "30/5/2;"
try 1 "30/5/2/2;" # rounded down
try 10 "40/2 -30/3*2 + 5*2;"
# introduced ()
try 6 "2*(2+1);"
try 1 "((4-2)*2 - 2)/(3-1);"
# introduced unary like -1, +3
try 5 "-3+8;"
try 6 "-2*(-3);"
try 0 "-(2+1)*+5+15;"
# introduced ==  operator
try 1 "1==1;"
try 0 "2==(3*2);"
# introduced !=  operator
try 0 "1!=1;"
try 1 "(2/1+1)!=(3*2);"
try 1 "((2!=(3*2))==1);"
# introduced < and <=  operator
try 1 "1<2;"
try 0 "1<1;"
try 1 "1<=2;"
try 1 "1<=1;"
try 0 "(2*3)<(4/2);"
# introduced < and <=  operator
try 0 "1>2;"
try 0 "1>1;"
try 1 "2>=2;"
try 1 "2>=1;"
try 1 "(2*3)>=(4/2);"
try 1 "((2*3+2/1)>=(4/2))<(2+-3+10);"
try 1 "((2*3+2/1)>=(4/2))<=(2<3+10);"
# local variable (one aphabet, small letter)
try 1 "a=1;"
try 5 "a=5;"
try 1 "a=5;a==5;"
try 0 "b=(2*(2+1));a=6;b==a+1;"
# local variable (multiple alphabet)
try 1 "aa=1;" 
try 26 "hoge=26;"
try 1 "hoge=9;fuga=9;hoge==fuga;"
try 1 "ho_ge=9;fu0GA=9;ho_ge==fu0GA;"
# add return statement
try 1 "return 1;"
try 3 "a=3;a;"
try 4 "hoge=4;return hoge;"
# add if statement
try 2 "if(1)2;"
try 5 "a=3;if(a)return 5;"
try 2 "b=4;if(b==4)return 2;"
try 5 "b=4;if(b==3)return 2;return 5;"
try 5 "a=9;b=4;if(b==3)if(a-5>0)return 2;return 5;"
try 2 "a=9;b=4;if(b==4)if(a-5>0)return 2;return 5;"
try 3 "if(a=2)return 3;return 4;"
try 4 "if(a=0)return 3;return 4;"
echo OK


