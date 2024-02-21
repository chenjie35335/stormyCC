  .text
  .globl main
main:
  li   ra, 6
  xor  ra, ra, x0
  seqz ra, ra

  sub  x0, x0, ra

  sub  x0, x0, gp

  xor  tp, tp, x0
  seqz tp, tp

  mv   a0, tp
  ret
