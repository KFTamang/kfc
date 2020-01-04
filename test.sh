#!/bin/bash

function fallback() {
echo "failed";
./kfc tmp.c > tmp.s && gcc -no-pie -o tmp test_helper.o tmp.s && ./tmp
}

./kfc tests.c > tmp.s && ./kfc stubs.c > tmp_stubs.s && gcc -no-pie -o tmp test_helper.o tmp.s tmp_stubs.s && ./tmp > tmp.c #|| fallback

