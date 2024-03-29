#include "common.h"
#include <unordered_map>
#pragma once

void Visit(const koopa_raw_return_t &ret,string &sign);
void Visit(const koopa_raw_value_t &value,string &sign);
void Visit(const koopa_raw_basic_block_t &bb,string &sign);
void Visit(const koopa_raw_function_t &func,string &sign);
void Visit(const koopa_raw_slice_t &slice,string &sign);
void Visit(const koopa_raw_binary_t &binary,string &sign);
void generateASM(const koopa_raw_program_t &program);
unordered_map <koopa_raw_value_t,int> stack_alloc;
unordered_map <koopa_raw_value_t,string> reg_alloc;
static int selected_reg = 0;
static int stack_offset = 0;
const char *regs[] = {
  "x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
bool reg_used[32] = {false};
string alloc_reg(){
    while(selected_reg <= 4  || selected_reg == 10 || selected_reg == 11 || reg_used[selected_reg]) {
        selected_reg++;
    }
    reg_used[selected_reg] = true;
    return regs[selected_reg++];
}

int alloc_stack() {
   return (stack_offset++)*4;
}