#include "common.h"
#pragma once

void Visit(const koopa_raw_return_t &ret);
void Visit(const koopa_raw_value_t &value);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_slice_t &slice);
void generateASM(const koopa_raw_program_t &program);