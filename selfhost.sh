#!/bin/bash

srcs1=(parse.c codegen.c)
srcs2=(tokenizer.c container.c main.c)

function preprocess(){
    header=${1/.c/.h}
    if [ $2 = 1 ];then
        cat $header $1 > tmp.c
    else
        cat $1 > tmp.c
    fi
    cat kfc.h tmp.c > result.c
    sed -i 's/, \.\.\.//g' result.c # ignore variable length argument function
    sed -i 's/VAR_NAME_SIZE/512/g' result.c
    sed -i 's/MAX_ARG_NUM/6/g' result.c 
    sed -i 's/MAX_SYMBOL_NUM/65535/g' result.c
    sed -i 's/stderr/2/g' result.c
    sed -i 's/va_start(ap, fmt)/ap=\&(fmt)+sizeof(fmt)/g' result.c
    sed -i 's/va_list/void*/g' result.c
    sed -i 's/FILE*/void*/g' result.c
    sed -i 's/EXIT_FAILURE/1/g' result.c
    sed -i 's/SEEK_END/2/g' result.c
    sed -i 's/SEEK_SET/0/g' result.c
    cat result.c
    rm tmp.c result.c
}

for src in ${srcs1[@]};do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
preprocess ${src} "1" > ${tmp}
./kfc ${tmp} > ${asm}
done


for src in ${srcs2[@]};do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
preprocess ${src} "0" > ${tmp}
./kfc ${tmp} > ${asm}
done

asms=(sh_main_1.s sh_parse_1.s sh_tokenizer_1.s sh_container_1.s sh_codegen_1.s)
gcc -no-pie -o kfc_gen1 ${asms[@]} -std=c11 -g
# echo ${asms[@]}

