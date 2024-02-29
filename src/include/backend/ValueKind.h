#include <iostream>
#include <stdint.h>
#include <string>
#include <IrGraph.h>
#include <ResourceAlloc.h>
using namespace std;
//原始值，这个是生成asm的关键部分
enum {
    /// Not equal to.
    RBO_NOT_EQ,
    /// Equal to.
    RBO_EQ,
/// Greater than.
    RBO_GT,
/// Less than.
    RBO_LT,
/// Greater than or equal to
    RBO_GE,
/// Less than or equal to.
    RBO_LE,
/// Addition.
    RBO_ADD,
/// Subtraction.    
    RBO_SUB,
/// Multiplication.
    RBO_MUL,
/// Division.
    RBO_DIV,
/// Modulo.
    RBO_MOD,
/// Bitwise AND.
    RBO_AND,
/// Bitwise OR.
    RBO_OR,
/// Bitwise XOR.
    RBO_XOR,
/// Shift left logical.
    RBO_SHL,
/// Shift right logical.
    RBO_SHR,
/// Shift right arithmetic.
    RBO_SAR,
}BinaryOp;

enum {
    REGISTER,
    MEMORY,
    UNALLOC
}RawValueStatus;

RegisterManager registerManager;
//肯定要建立的是原始值和寄存器，内存之间的对应关系
//建立的方法是
class RawValue : public midend {
    public:
        char *name;
        RawSlice used_by;
        ValueKind* value;
        int used_times;
        Register reg;
        Memory memory;
    private:
        int Status
        //RawType ty;
        Resource Visit() {
            switch(Status) {
                case REGISTER:
                    return reg; 
                    break;
            }
        }

        void setStatus(int status){
            Status = status;
        }
};
//各类变量的总父类，作为接口
class ValueKind {
    public:
       virtual void Visit(string &sign) const = 0;
};
//整型变量
class RawInteger  : public ValueKind {
    public:
    int32_t value;
    void Visit() const override {
        
    }
};
//浮点型变量
class RawFloat : public ValueKind {
    public:
    float value;
    void Visit() const override {

    }
}
//返回值
class RawReturn : public ValueKind {
    public:
    RawValue *value;
    void Visit() const override {

    }
};

//运算
class RawBinary : public ValueKind {
    public:
    uint32_t op;
    RawValue *lhs;
    RawValue *rhs;
    void Visit() const override {

    }
};
//读内存
class RawLoad : public ValueKind {
    public:
    RawValue *src;
    void Visit() const override {

    }
};
//写内存
class RawStore : public ValueKind {
    public:
    RawValue *value;
    RawValue *dest;
    void Visit() const override {
        
    }
}

/*
    考虑在一个基本块中的情况：
        1、对于一个原始值，既可以存放在内存栈中，也可以存放在寄存器中
        2、如果寄存器需要腾出来，我从原始值表中找到最近使用最少的原始值（LRU）
        3、一个寄存器可以存储多个值，但是一个值只能存在一个寄存器和一个内存当中
    结论：
        也就是说我需要在原始值内部声明一个寄存器和一个内存的对象，这个用来表示分配
        给当前原始值的寄存器和内存。但是真实情况下到底是使用内存还是寄存器，需要寄存器分配器
        和内存分配器来决定，内存分配器会调用原始值表，动态地分配。也就是说，这两个分配器来决定
        原始值的类型
        所以说：
        1、 原始值中需要两个变量来存储内存信息和寄存器信息
        2、 原始值中需要一个变量来存储原始值状态
        3、 初始时刻原始值状态为unalloc,然后每次遍历改变状态
        4、 寄存器分配器和栈分配器可能需要维护一个列表，用于存储所有原始值的引用，
        5、 原始值应该也要定义一个函数用来完成保存功能的实现
        6、 我的方法是： 铁打的内存， 不断变化的寄存器
*/