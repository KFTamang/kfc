#!/bin/bash

srcs=(parse.c tokenizer.c container.c codegen.c main.c)

for src in ${srcs[@]};do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
obj=sh_${src/.c/_1.o}
cat ${src/.c/.h} ${src} > tmp
cat kfc.h tmp > ${tmp}
sed -i 's/, \.\.\.//g' ${tmp}
./kfc ${tmp} > ${asm}
gcc -c ${asm} -o ${obj}
done

rm tmp

# objs=(sh_parse_1.o sh_tokenizer_1.o sh_container_1.o sh_codegen_1.o sh_main_1.o)
# gcc ${objs} -o kfc_gen1