#include "IR.h"
void backend(char *str,const char *output)
{
    
    koopa_program_t program = NULL;
    printf("begin parse\n");
    koopa_error_code_t ret = koopa_parse_from_string(str,&program);
    printf("end parse1\n");
    assert(ret == KOOPA_EC_SUCCESS);
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    printf("end parse2\n");
    koopa_raw_program_t raw = koopa_build_raw_program(builder,program);
    printf("end parse3\n");
    koopa_delete_program(program);
    printf("end parse4\n");
    freopen(output,"w",stdout);
    generateASM(raw);
    koopa_delete_raw_program_builder(builder);
}

void Visit(const koopa_raw_integer_t &integer)
{
    printf("  li a0, %d\n",integer.value);
}

void Visit(const koopa_raw_return_t &ret)
{
    Visit(ret.value);
}

void Visit(const koopa_raw_value_t &value) {
    const auto& kind = value->kind;
    switch(kind.tag) {
        case KOOPA_RVT_RETURN: {
            Visit(kind.data.ret);
            printf("  ret");
            break;
        }
        case KOOPA_RVT_INTEGER: {
            Visit(kind.data.integer);
            break;
        }
        default:
            assert(false);
    }
} 

void Visit(const koopa_raw_basic_block_t &bb){
    Visit(bb->insts);
} 

void Visit(const koopa_raw_function_t &func)
{
        printf("  .globl %s\n",func->name+1);
        printf("%s:\n",func->name+1);
        Visit(func->bbs);
}

void Visit(const koopa_raw_slice_t &slice){
    for(size_t i = 0; i < slice.len; i++) {
        auto ptr = slice.buffer[i];
        switch(slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                //printf("begin parse function\n");
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr));break;
            case KOOPA_RSIK_BASIC_BLOCK:
                //printf("begin parse block\n");
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));break;
            case KOOPA_RSIK_VALUE:
                //printf("begin parse value\n");
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr));break;
            default:
                assert(false);
        }
    }
}

void generateASM(const koopa_raw_program_t &program)
{
    printf("  .text\n");
    Visit(program.funcs);
}
















