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

try 0 0
try 45 45
try 6 '5+10-9'
try 178 "80 + 100 -2"
try 198 "100 + 100 - 2"
try 21  "1 + 2 + 3 + 4 + 5 + 6"
try 12 "30 - 3 - 4 -5-6"
#try -16 "2 - 3 - 4 -5-6" # negative number not yet implemented
try 150 "90 - 10 + 120 - 30 + 10 - 20 + 30 - 40"
try 0 "90-100+1+14-29+24"
# introduced multiplication
try 2 "1*2"
try 42 "2*20 + 2"
try 30 "5*2 + 10*2"
try 35 "5 + 1* 2*5+10 *2"
# introduced division
try 6 "30/5"
try 12 "30/5*2"
try 3 "30/5/2"
try 1 "30/5/2/2" # rounded down
try 10 "40/2 -30/3*2 + 5*2"
# introduced ()
try 6 "2*(2+1)"
try 1 "((4-2)*2 - 2)/(3-1)"
# introduced unary like -1, +3
try 5 "-3+8"
try 6 "-2*(-3)"
try 0 "-(2+1)*+5+15"
echo OK


