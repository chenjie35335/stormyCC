#include "common.h"
#include <unordered_map>
#pragma once
int Cal_Stack_Size(const koopa_raw_function_t &func,bool &has_call);

void Visit_integer(const koopa_raw_integer_t &integer,const koopa_raw_value_t &value);
void Visit_alloc(const koopa_raw_value_t &value);
void Visit_func_arg_ref(const koopa_raw_func_arg_ref_t &func_arg_ref,const koopa_raw_value_t &value);
void Visit_store(const koopa_raw_store_t &store,const koopa_raw_value_t &value);
void Visit_load(const koopa_raw_load_t &load,const koopa_raw_value_t &value);
void Visit_binary(const koopa_raw_binary_t &binary,const koopa_raw_value_t &value);
void Visit_branch(const koopa_raw_branch_t &branch);
void Visit_jump(const  koopa_raw_jump_t &jump);
void Visit_call(const koopa_raw_call_t &call,const koopa_raw_value_t &value);
void Visit_return(const koopa_raw_return_t &ret);
void Visit_value(const koopa_raw_value_t &value);
void Visit_bbs(const koopa_raw_basic_block_t &bb);
void Visit_function(const koopa_raw_function_t &func);
void Visit_slice(const koopa_raw_slice_t &slice);
void generateASM(const koopa_raw_program_t &program);
unordered_map <koopa_raw_value_t,int> stack_alloc;
unordered_map <koopa_raw_value_t,string> reg_alloc;
bool has_call = false;
static int Need_Size_RSA_16 = 0;
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