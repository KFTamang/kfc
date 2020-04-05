#!/bin/bash

srcs=(parse.c tokenizer.c container.c codegen.c main.c)

for src in parse.c;do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
obj=sh_${src/.c/_1.o}
cat kfc.h ${src} > ${tmp}
sed -i 's/, \.\.\.//g' ${tmp}
./kfc ${tmp} > ${asm}
gcc ${asm} -o ${obj}
done

# gcc ${srcs} -o kfc_gen1