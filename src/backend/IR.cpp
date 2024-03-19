#include "IR.h"
extern int selected_reg;
//有关于变量的部分，需要用到内存相关的内容，这里需要一个映射来表示一下内容
//这个映射存储着表示一个变量和变量关于sp的偏移量相关的内容
//同时需要维护一个全局变量表示当前可分配的栈的位置（注意，由于我们这里使用的是面向过程的方式，所以这里用全局变量，过几天这里将全部改为面向对象）
void backend(char *str,const char *output)
{
    koopa_program_t program;
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
void Visit_integer(const koopa_raw_integer_t &integer,const koopa_raw_value_t &value)
{
    //printf("parse integer\n");
    //printf("  li a0, %d\n",integer.value);

    string sign;
    if(integer.value == 0) {
        sign = regs[0];
    }
    else {
        //printf("parse non-zero integer\n");
        sign = alloc_reg();
        printf("  li   %s, %d\n",sign.c_str(),integer.value);
        //cout << "  li" + sign + "," + to_string(integer.value) << endl;
    }
    reg_alloc.insert(pair<koopa_raw_value_t,string>(value,sign));
}
//raw_alloc
void Visit_alloc(const koopa_raw_value_t &value)
{
    int offset = alloc_stack();
    stack_alloc.insert(pair<koopa_raw_value_t,int>(value,offset));
}
//raw_return
void Visit_return(const koopa_raw_return_t &ret)
{
    Visit_value(ret.value);
    // for(int i = 0; i < 32; i++) {
    //     if(!sign.compare(regs[i]) && sign.compare("a0")) {
            cout << "  mv   a0, " + reg_alloc[ret.value] << endl; //break;
            cout << "  addi sp, sp, 256" <<  endl;
            cout << "  ret" << endl;
    //     }
    // }
}
//raw_binary
void Visit_binary(const koopa_raw_binary_t &binary,const koopa_raw_value_t &value) {
    //printf("begin parse binary\n");
    const auto & binary_op = binary.op;
    Visit_value(binary.lhs);
    Visit_value(binary.rhs);
    string s1 = reg_alloc[binary.lhs], s2 = reg_alloc[binary.rhs];
    string sign = alloc_reg();
    //cout << s1 << s2 <<endl;
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
    reg_alloc.insert(pair<koopa_raw_value_t,string>(value,sign));
}
//raw_store
void Visit_store(const koopa_raw_store_t &store,const koopa_raw_value_t &value) {
    Visit_value(store.value);
    Visit_value(store.dest);
    string sign1 = reg_alloc.at(store.value);
    string sign2 = to_string(stack_alloc.at(store.dest))+"(sp)";
    cout << "  sw   " << sign1 << ", " <<sign2 <<endl;
}
//raw_load
void Visit_load(const koopa_raw_load_t &load,const koopa_raw_value_t &value) {
    string sign = to_string(stack_alloc.at(load.src))+"(sp)";
    string sign1 = alloc_reg();
    cout << "  lw   " << sign1 << ", " <<sign <<endl;
    sign = sign1;
    reg_alloc.insert(pair<koopa_raw_value_t,string>(value,sign));
}
void Vist_branch(const koopa_raw_branch_t &branch)
{
    // cout<<"Vist_branch"<<endl;
    // cout<<"cond:{"<<reg_alloc[branch.cond]<<"}"<<endl;
    // cout<<"true_bb:{"<<branch.true_bb->name+1<<"}"<<endl;
    // cout<<"false_bb:{"<<branch.false_bb->name+1<<"}"<<endl;
    Visit_value(branch.cond);
    cout<<"  bnez "<<reg_alloc[branch.cond]<<", "<<branch.true_bb->name+1<<endl;
    cout<<"  j "<<branch.false_bb->name+1<<endl;
}
void Visit_jump(const  koopa_raw_jump_t &jump)
{
    // cout<<"Visit_jump"<<endl;
    // cout<<"Jump_target:{"<<jump.target->name+1<<"}\n"<<endl;
    cout<<"  j "<<jump.target->name+1<<endl;
}
//现在不清楚的是这里是图还是树，
//现在是想办法建立树节点与分配的寄存器之间的关系，从而实现剪枝，避免多余
//这里将寄存器分配和这个koopa_raw_value_t绑定在一起试试
//raw_value
void Visit_value(const koopa_raw_value_t &value) {
    // printf("value = %s\n",value->name);
    // if(reg_alloc.find(value) != reg_alloc.end()) {
    //     sign = reg_alloc.at(value);
    //     return;
    // }
    // else if(stack_alloc.find(value) != stack_alloc.end()){
    //     sign = to_string(stack_alloc.at(value))+"(sp)";
    //     return;
    // }
    // else{
    if(reg_alloc.find(value) != reg_alloc.end())
        return ;
    if(stack_alloc.find(value) != stack_alloc.end())
        return;
    const auto& kind = value->kind;
    switch(kind.tag) {
        case KOOPA_RVT_RETURN: {
            // printf("parse return\n");
            Visit_return(kind.data.ret);
            break;
        }
        case KOOPA_RVT_INTEGER: {
            // printf("parse integer\n");
            Visit_integer(kind.data.integer,value);
            break;
        }
        case KOOPA_RVT_BINARY: {
            //printf("parse binary\n");
            Visit_binary(kind.data.binary,value);
            break;
        }
        case KOOPA_RVT_ALLOC: {
            //printf("parse alloc\n");
            Visit_alloc(value);
            break;
        }
        case KOOPA_RVT_LOAD: {
            //printf("parse load\n");
            Visit_load(kind.data.load,value);
            break;
        }
        case KOOPA_RVT_STORE: {
            //printf("parse store\n");
            Visit_store(kind.data.store,value);
            break;
        }
        case KOOPA_RVT_BRANCH:{
            // cout<<"parse branch"<<endl;
            Vist_branch(kind.data.branch);
            break;
        }
        case KOOPA_RVT_JUMP:{
            // cout<<"parse jump"<<endl;
            Visit_jump(kind.data.jump);
            break;
        }
        default:
            assert(false);
    }
} 
//raw_basic_block
void Visit_bbs(const koopa_raw_basic_block_t &bb){
    //basic_block_name
    cout<<endl;
    cout<<bb->name+1<<":"<<endl;
    // printf("%s:\n",bb->name+1);
    Visit_slice(bb->insts);
} 
//raw_function
//这里先只做一个map,如果之后使用多函数，可能会使用map来存储（后话暂且不说）
void Visit_function(const koopa_raw_function_t &func)
{
        printf("  .globl %s\n",func->name+1);
        printf("%s:\n",func->name+1);
        cout << "  addi sp, sp, -256" <<  endl;
        stack_offset = 0;
        Visit_slice(func->bbs);
       
}
//这里我没有遍历所有的，是因为如果遍历会出现重复打印，可能之后还会修改
void Visit_slice(const koopa_raw_slice_t &slice){
    for(size_t i = 0; i < slice.len; i++) {
        //cout << endl;
        // printf("slice.len == %d, i == %d\n",slice.len,i);
        //auto ptr = slice.buffer[slice.len-1];
        auto ptr = slice.buffer[i];
        switch(slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                //printf("begin parse function\n");
                Visit_function(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                // printf("begin parse block\n");
                // cout<<"slice.len =="<<slice.len<<", i == "<<i<<endl;
                Visit_bbs(reinterpret_cast<koopa_raw_basic_block_t>(ptr));break;
            case KOOPA_RSIK_VALUE:
                //printf("begin parse value\n");
                Visit_value(reinterpret_cast<koopa_raw_value_t>(ptr));break;
            default:
                assert(false);
        }
    }
}

void generateASM(const koopa_raw_program_t &program)
{
    selected_reg = 0;
    printf("  .text\n");
    Visit_slice(program.funcs);
}
















