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
try 2 "1*2"
try 42 "2*20 + 2"
try 30 "5*2 + 10*2"
try 35 "5 + 1* 2*5+10 *2"
echo OK


