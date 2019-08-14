.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  call test
  pop rax
  mov rsp, rbp
  pop rbp
  ret
