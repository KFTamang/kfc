#!/bin/bash

srcs=(parse.c tokenizer.c container.c codegen.c main.c)

for src in parse.c;do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
obj=sh_${src/.c/_1.o}
cat ${src/.c/.h} ${src} > tmp
cat kfc.h tmp > ${tmp}
sed -i 's/, \.\.\.//g' ${tmp}
./kfc ${tmp} > ${asm}
gcc ${asm} -o ${obj}
done

rm tmp

# gcc ${srcs} -o kfc_gen1