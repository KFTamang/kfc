#!/bin/bash

srcs1=(parse.c codegen.c)
srcs2=(tokenizer.c container.c main.c)


for src in ${srcs1[@]};do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
obj=sh_${src/.c/_1.o}
cat ${src/.c/.h} ${src} > tmp
cat kfc.h tmp > ${tmp}
sed -i 's/, \.\.\.//g' ${tmp} # ignore variable length argument function
sed -i 's/VAR_NAME_SIZE/512/g' ${tmp}
sed -i 's/MAX_ARG_NUM/6/g' ${tmp} 
sed -i 's/MAX_SYMBOL_NUM/65535/g' ${tmp}
./kfc ${tmp} > ${asm}
gcc -c ${asm} -o ${obj}
done


for src in ${srcs2[@]};do
tmp=sh_${src/.c/_1.c}
asm=sh_${src/.c/_1.s}
obj=sh_${src/.c/_1.o}
cat ${src} > tmp
cat kfc.h tmp > ${tmp}
sed -i 's/, \.\.\.//g' ${tmp} # ignore variable length argument function
sed -i 's/VAR_NAME_SIZE/512/g' ${tmp}
sed -i 's/MAX_ARG_NUM/6/g' ${tmp} 
sed -i 's/MAX_SYMBOL_NUM/65535/g' ${tmp}
./kfc ${tmp} > ${asm}
gcc -c ${asm} -o ${obj}
done

rm tmp

# objs=(sh_parse_1.o sh_tokenizer_1.o sh_container_1.o sh_codegen_1.o sh_main_1.o)
# gcc ${objs} -o kfc_gen1