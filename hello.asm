  .text
  .globl half
half:
  addi sp, sp, -16
  sw   a0, 0(sp)
  lw   t0, 0(sp)
  li   t1, 2
  div  t2, t0, t1
  mv   a0, t2
  addi sp, sp, 16
  ret

  .text
  .globl f
f:
  addi sp, sp, 0
  ret

  .text
  .globl main
main:
  addi sp, sp, -16
  sw ra, 12(sp)
  call f
  li   s0, 10
  mv a0, s0
  call half
  sw a0, 4(sp)
  mv   a0, 4(sp)
  lw ra, 12(sp)
  addi sp, sp, 16
  ret

