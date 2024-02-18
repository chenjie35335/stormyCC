  .text
  .globl main
main:
  li   ra, 2
  li   sp, 1
  li   gp, 1
  div  tp, sp, gp

  add  t0, ra, tp

  mv   a0, t0
  ret
