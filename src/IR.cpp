#include "IR.h"
extern int selected_reg;
//有关于变量的部分，需要用到内存相关的内容，这里需要一个映射来表示一下内容
//这个映射存储着表示一个变量和变量关于sp的偏移量相关的内容
//同时需要维护一个全局变量表示当前可分配的栈的位置（注意，由于我们这里使用的是面向过程的方式，所以这里用全局变量，过几天这里将全部改为面向对象）
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
        cout << "  xor  " +sign+", "+ s1 + ", " +s2 <<endl;
        cout << "  seqz " +sign+", "+ sign  <<endl;
        break;
        case KOOPA_RBO_NOT_EQ : 
//printf("parsing eq\n");
        cout << "  xor  " +s1+", "+ s1 + ", " +s2 <<endl;
        cout << "  snez " +s1+", "+ s1  <<endl;
        sign = s1;
        break;
        case KOOPA_RBO_MUL :
        cout << "  mul  " + sign + ", " + s1 + ", "+s2<<endl;
        break;
        case KOOPA_RBO_DIV :
        cout << "  div  " + sign + ", " + s1 + ", "+s2 << endl;
        break; 
        case KOOPA_RBO_MOD :
        cout << "  rem  " + sign + ", " + s1 + ", "+s2 << endl;
        break; 
        case KOOPA_RBO_LT :
        cout << "  slt  " + s1 + ", " + s1 + ", "+s2 << endl;
        sign = s1; break;
        case KOOPA_RBO_GT :
        cout << "  slt  " + s1 + ", " + s2 + ", "+s1 << endl;
        sign = s1; break;
        case KOOPA_RBO_GE:
        cout << "  slt  " + s1 + ", " + s1 + ", "+s2 << endl;
        cout << "  seqz " + s1 + ", " + s1  <<endl;
        sign = s1;      
        break;
        case KOOPA_RBO_LE:
        cout << "  sgt  " + s1 + ", " + s1 + ", "+s2 << endl;
        cout << "  seqz " + s1 + ", " + s1  <<endl;
        sign = s1;      
        break;
        case KOOPA_RBO_OR:
        cout << "  or  " + sign + ", " + s1 + ", "+s2 << endl;
        break;
        case KOOPA_RBO_XOR:
        cout << "  xor  " + sign + ", " + s1 + ", "+s2 << endl;
        break;
        case KOOPA_RBO_AND:
        cout << "  and  " + sign + ", " + s1 + ", "+s2 << endl;
        break;
        default:assert(false);
    }
    cout << endl;
}

void Visit(const koopa_raw_store_t &store,string &sign) {
    string sign1,sign2;
    Visit(store.value,sign1);
    Visit(store.dest,sign2);
}

void Visit(const koopa_raw_load_t &load,string &sign) {
    Visit(load.src,sign);
}
//现在不清楚的是这里是图还是树，
//现在是想办法建立树节点与分配的寄存器之间的关系，从而实现剪枝，避免多余
//这里将寄存器分配和这个koopa_raw_value_t绑定在一起试试
//raw_value
void Visit(const koopa_raw_value_t &value,string &sign) {
    //printf("value = %s\n",value->name);
    if(reg_alloc.find(value) != reg_alloc.end()) {
        sign = reg_alloc.at(value);
        return;
    }
    else{
    const auto& kind = value->kind;
    switch(kind.tag) {
        case KOOPA_RVT_RETURN: {
            //printf("parse return\n");
            Visit(kind.data.ret,sign);
            cout << "  addi sp, sp, 256" <<  endl;
            cout << "  ret" << endl;
            break;
        }
        case KOOPA_RVT_INTEGER: {
            //printf("parse integer\n");
            Visit(kind.data.integer,sign);
            break;
        }
        case KOOPA_RVT_BINARY: {
            //printf("parse binary\n");
            Visit(kind.data.binary,sign);
            break;
        }
        case KOOPA_RVT_ALLOC: {
            //printf("parse alloc\n");
            
            break;
        }
        case KOOPA_RVT_LOAD: {
            //printf("parse load\n");
            Visit(kind.data.load,sign);
            break;
        }
        case KOOPA_RVT_STORE: {
            //printf("parse store\n");
            Visit(kind.data.store,sign);
            break;
        }
        default:
            assert(false);
    }
    reg_alloc.insert(pair<koopa_raw_value_t,string>(value,sign));
    }
} 
//raw_basic_block
void Visit(const koopa_raw_basic_block_t &bb,string &sign){
    Visit(bb->insts,sign);
} 
//raw_function
//这里先只做一个map,如果之后使用多函数，可能会使用map来存储（后话暂且不说）
void Visit(const koopa_raw_function_t &func,string &sign)
{
        printf("  .globl %s\n",func->name+1);
        printf("%s:\n",func->name+1);
        cout << "  addi sp, sp, -256" <<  endl;
        stack_offset = 0;
        Visit(func->bbs,sign);
       
}
//这里我没有遍历所有的，是因为如果遍历会出现重复打印，可能之后还会修改
void Visit(const koopa_raw_slice_t &slice,string &sign){
    for(size_t i = 0; i < slice.len; i++) {
        //printf("slice.len == %d, i == %d\n",slice.len,i);
        //auto ptr = slice.buffer[slice.len-1];
        auto ptr = slice.buffer[i];
        switch(slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                //printf("begin parse function\n");
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr),sign);
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                //printf("begin parse block\n");
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr),sign);break;
            case KOOPA_RSIK_VALUE:
                //printf("begin parse value\n");
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr),sign);break;
            default:
                assert(false);
        }
    }
}

void generateASM(const koopa_raw_program_t &program)
{
    char *c = "si";
    string sign(c);
    selected_reg = 0;
    printf("  .text\n");
    Visit(program.funcs,sign);
}
















