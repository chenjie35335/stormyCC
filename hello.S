  .text
  .globl main
main:
  addi sp, sp, -256
  li   t0, 3
  li   t1, 4
  add  t2, t0, t1

  li   s0, 2
  mul  s1, s0, t2

  li   a2, 5
  div  a3, s1, a2

  li   a4, 1
  add  a5, a4, a3

  mv   a0, a5
  addi sp, sp, 256
  ret
