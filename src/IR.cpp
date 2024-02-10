#include "IR.h"
extern int selected_reg;
void backend(char *str,const char *output)
{
    koopa_program_t program = NULL;
    //printf("begin parse\n");
    koopa_error_code_t ret = koopa_parse_from_string(str,&program);
    //printf("end parse1\n");
    assert(ret == KOOPA_EC_SUCCESS);
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    //printf("end parse2\n");
    koopa_raw_program_t raw = koopa_build_raw_program(builder,program);
    //printf("end parse3\n");
    koopa_delete_program(program);
    //printf("end parse4\n");
    freopen(output,"w",stdout);
    generateASM(raw);
    koopa_delete_raw_program_builder(builder);
}
//raw_integer
void Visit(const koopa_raw_integer_t &integer,string &sign)
{
    //printf("parse integer\n");
    //printf("  li a0, %d\n",integer.value);
    if(integer.value == 0) {
        sign = regs[0];
    }
    else {
        //printf("parse non-zero integer\n");
        sign = alloc_reg();
        printf("  li   %s, %d\n",sign.c_str(),integer.value);
        //cout << "  li" + sign + "," + to_string(integer.value) << endl;
    }
}
//raw_return
void Visit(const koopa_raw_return_t &ret,string &sign)
{
    Visit(ret.value,sign);
    for(int i = 0; i < 32; i++) {
        if(!sign.compare(regs[i]) && sign.compare("a0")) {
            cout << "  mv   a0, " + sign << endl; break;
        }
    }
}
//raw_binary
void Visit(const koopa_raw_binary_t &binary,string &sign) {
    //printf("begin parse binary\n");
    const auto & binary_op = binary.op;
    string s1 = "a0", s2 = "a0";
    Visit(binary.lhs,s1);
    Visit(binary.rhs,s2);
    //cout << s1 << s2 <<endl;
    sign = alloc_reg();
    switch(binary_op){
        case KOOPA_RBO_ADD: 
        //printf("parsing add\n");
        cout << "  add  " +sign+", "+ s1 + ", " +s2 <<endl; 
        break;
        case KOOPA_RBO_SUB: 
        //printf("parsing sub\n");
        cout << "  sub  " +sign+", "+ s1 + ", " +s2 <<endl;
        break;
        case KOOPA_RBO_EQ : 
        //printf("parsing eq\n");
        cout << "  xor  " +s1+", "+ s1 + ", " +s2 <<endl;
        cout << "  seqz " +s1+", "+ s1  <<endl;
        sign = s1;
        break;
        default:assert(false);
    }
    cout << endl;
}
//raw_value
void Visit(const koopa_raw_value_t &value,string &sign) {
    const auto& kind = value->kind;
    switch(kind.tag) {
        case KOOPA_RVT_RETURN: {
           // printf("parse return\n");
            Visit(kind.data.ret,sign);
            printf("  ret\n");
            break;
        }
        case KOOPA_RVT_INTEGER: {
           // printf("parse integer\n");
            Visit(kind.data.integer,sign);
            break;
        }
        case KOOPA_RVT_BINARY: {
            //printf("parse binary\n");
            Visit(kind.data.binary,sign);
            break;
        }
        default:
            assert(false);
    }
} 
//raw_basic_block
void Visit(const koopa_raw_basic_block_t &bb,string &sign){
    Visit(bb->insts,sign);
} 
//raw_function
void Visit(const koopa_raw_function_t &func,string &sign)
{
        printf("  .globl %s\n",func->name+1);
        printf("%s:\n",func->name+1);
        Visit(func->bbs,sign);
}
//这里我没有遍历所有的，是因为如果遍历会出现重复打印，可能之后还会修改
void Visit(const koopa_raw_slice_t &slice,string &sign){
    //for(size_t i = 0; i < slice.len; i++) {
        //printf("slice.len == %d, i == %d\n",slice.len,i);
        auto ptr = slice.buffer[slice.len-1];
        switch(slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                //printf("begin parse function\n");
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr),sign);break;
            case KOOPA_RSIK_BASIC_BLOCK:
                //printf("begin parse block\n");
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr),sign);break;
            case KOOPA_RSIK_VALUE:
                //printf("begin parse value\n");
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr),sign);break;
            default:
                assert(false);
        }
    //}
}

void generateASM(const koopa_raw_program_t &program)
{
    char *c = "si";
    string sign(c);
    selected_reg = 0;
    printf("  .text\n");
    Visit(program.funcs,sign);
}
















