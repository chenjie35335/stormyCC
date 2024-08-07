#pragma once

#include "ast/base_ast.hpp"

class ExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> unaryExp;

    ExpAST(std::unique_ptr<BaseAST> &_unaryExp)
    {
        unaryExp = std::move(_unaryExp);
    }

    void *build_koopa_values() const override
    {
        return unaryExp->build_koopa_values();
    }

    int CalcValue() const override
    {
        return unaryExp->CalcValue();
    }
};

class LValAST : public BaseAST
{
    enum ValType
    {
        Num,
        Array
    };
public:
    ValType type;
    std::string name;
    std::vector<std::unique_ptr<BaseAST>> idx;
    LValAST(const char *_name) : name(_name) 
    {
        type = Num;
    }
    LValAST(const char *_name, std::vector<BaseAST*> &_idx) : name(_name)
    {
        type = Array;
        for(auto &i : _idx)
            idx.emplace_back(i);
    } 

    // 将变量作为左值返回（返回该左值的变量本身）
    void *koopa_leftvalue() const override
    {
        if(type == Array)
        {
            koopa_raw_value_data *get;
            koopa_raw_value_t src = (koopa_raw_value_t)symbol_list.GetSymbol(name).number;
            if(src->ty->data.pointer.base->tag == KOOPA_RTT_POINTER)
            {
                koopa_raw_value_t src = (koopa_raw_value_t)symbol_list.GetSymbol(name).number;
                koopa_raw_value_data *load0 = new koopa_raw_value_data();
                load0->ty = src->ty->data.pointer.base;
                load0->name = nullptr;
                load0->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                load0->kind.tag = KOOPA_RVT_LOAD;
                load0->kind.data.load.src = src;
                block_maintainer.AddInst(load0);

                bool first = true;
                src = load0;
                for(auto &i : idx)
                {
                    get = new koopa_raw_value_data();
                    koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                    if(first)
                    {
                        get->ty = src->ty;
                        get->name = nullptr;
                        get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                        get->kind.tag = KOOPA_RVT_GET_PTR;
                        get->kind.data.get_ptr.src = src;
                        get->kind.data.get_ptr.index = (koopa_raw_value_t)i->build_koopa_values();
                        first = false;
                    }
                    else
                    {
                        ty->tag = KOOPA_RTT_POINTER;
                        ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                        get->ty = ty;
                        get->name = nullptr;
                        get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                        get->kind.tag = KOOPA_RVT_GET_ELEM_PTR;
                        get->kind.data.get_elem_ptr.src = src;
                        get->kind.data.get_elem_ptr.index = (koopa_raw_value_t)i->build_koopa_values();
                    }
                    block_maintainer.AddInst(get);
                    src = get;
                }
            }
            else
            {
                for(auto &i : idx)
                {
                    get = new koopa_raw_value_data();
                    koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                    ty->tag = KOOPA_RTT_POINTER;
                    ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                    get->ty = ty;
                    get->name = nullptr;
                    get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                    get->kind.tag = KOOPA_RVT_GET_ELEM_PTR;
                    get->kind.data.get_elem_ptr.src = src;
                    get->kind.data.get_elem_ptr.index = (koopa_raw_value_t)i->build_koopa_values();
                    block_maintainer.AddInst(get);
                    src = get;
                }
            }
            return get;
        }
        else if (type == Num)
        {
            return (void *)symbol_list.GetSymbol(name).number;
        }
        return nullptr;
    }

    // 将变量作为右值返回（读取变量里存储的值）
    void *build_koopa_values() const override
    {
        koopa_raw_value_data *res = new koopa_raw_value_data();
        auto var = symbol_list.GetSymbol(name);
        if (var.type == LValSymbol::Const)
        //这里应该是LvalSymbol那里进行的改进，
        //在进入之前提前先设计好
            return (void *)var.number;
        else if (var.type == LValSymbol::Var)
        {
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_LOAD;
            res->kind.data.load.src = (koopa_raw_value_t)var.number;
            block_maintainer.AddInst(res);
        }
        else if (var.type == LValSymbol::Array)
        {
            bool need_load = false;
            koopa_raw_value_data *get;
            koopa_raw_value_data *src = (koopa_raw_value_data*)var.number;
            if(idx.empty())
            {
                get = new koopa_raw_value_data();
                koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                ty->tag = KOOPA_RTT_POINTER;
                ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                get->ty = ty;
                get->name = nullptr;
                get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                get->kind.tag = KOOPA_RVT_GET_ELEM_PTR;
                get->kind.data.get_elem_ptr.src = src;
                get->kind.data.get_elem_ptr.index = make_koopa_interger(0);
                block_maintainer.AddInst(get);
            }
            else
            {
                for(auto &i : idx)
                {
                    get = new koopa_raw_value_data();
                    koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                    ty->tag = KOOPA_RTT_POINTER;
                    ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                    get->ty = ty;
                    get->name = nullptr;
                    get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                    get->kind.tag = KOOPA_RVT_GET_ELEM_PTR;
                    get->kind.data.get_elem_ptr.src = src;
                    get->kind.data.get_elem_ptr.index = (koopa_raw_value_t)i->build_koopa_values();
                    block_maintainer.AddInst(get);
                    src = get;
                    if(ty->data.pointer.base->tag == KOOPA_RTT_INT32)
                        need_load = true;
                }
            }
            if(need_load)
            {
                res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
                res->name = nullptr;
                res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                res->kind.tag = KOOPA_RVT_LOAD;
                res->kind.data.load.src = get;
                block_maintainer.AddInst(res);
            }
            else if(src->ty->data.pointer.base->tag == KOOPA_RTT_ARRAY)
            {
                res = new koopa_raw_value_data();
                koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                ty->tag = KOOPA_RTT_POINTER;
                ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                res->ty = ty;
                res->name = nullptr;
                res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                res->kind.tag = KOOPA_RVT_GET_ELEM_PTR;
                res->kind.data.get_elem_ptr.src = src;
                res->kind.data.get_elem_ptr.index = make_koopa_interger(0);
                block_maintainer.AddInst(res);
            }
            else
                res = src;
        }
        else if (var.type == LValSymbol::Pointer)
        {
            koopa_raw_value_data *src = (koopa_raw_value_data*)var.number;
            koopa_raw_value_data *load0 = new koopa_raw_value_data();
            load0->ty = src->ty->data.pointer.base;
            load0->name = nullptr;
            load0->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            load0->kind.tag = KOOPA_RVT_LOAD;
            load0->kind.data.load.src = src;
            block_maintainer.AddInst(load0);

            bool need_load = false;
            bool first = true;
            koopa_raw_value_data *get;
            src = load0;
            for(auto &i : idx)
            {
                get = new koopa_raw_value_data();
                koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                if(first)
                {
                    get->ty = src->ty;
                    get->name = nullptr;
                    get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                    get->kind.tag = KOOPA_RVT_GET_PTR;
                    get->kind.data.get_ptr.src = src;
                    get->kind.data.get_ptr.index = (koopa_raw_value_t)i->build_koopa_values();
                    first = false;
                }
                else
                {
                    ty->tag = KOOPA_RTT_POINTER;
                    ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                    get->ty = ty;
                    get->name = nullptr;
                    get->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                    get->kind.tag = KOOPA_RVT_GET_ELEM_PTR;
                    get->kind.data.get_elem_ptr.src = src;
                    get->kind.data.get_elem_ptr.index = (koopa_raw_value_t)i->build_koopa_values();
                }
                block_maintainer.AddInst(get);
                src = get;
                if(get->ty->data.pointer.base->tag == KOOPA_RTT_INT32)
                    need_load = true;
            }
            if(need_load)
            {
                res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
                res->name = nullptr;
                res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                res->kind.tag = KOOPA_RVT_LOAD;
                res->kind.data.load.src = get;
                block_maintainer.AddInst(res);
            }
            else if(src->ty->data.pointer.base->tag == KOOPA_RTT_ARRAY)
            {
                res = new koopa_raw_value_data();
                koopa_raw_type_kind *ty = new koopa_raw_type_kind();
                ty->tag = KOOPA_RTT_POINTER;
                ty->data.pointer.base = src->ty->data.pointer.base->data.array.base;
                res->ty = ty;
                res->name = nullptr;
                res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
                res->kind.tag = KOOPA_sEM_PTR;
                res->kind.data.get_elem_ptr.src = src;
                res->kind.data.get_elem_ptr.index = make_koopa_interger(0);
                block_maintainer.AddInst(res);
            }
            else
                res = src;
        }
        return res;
    }

    int CalcValue() const override
    {
        auto var = symbol_list.GetSymbol(name);
        assert(var.type == LValSymbol::Const);
        return ((koopa_raw_value_t)var.number)->kind.data.integer.value;
    }
};

class PrimaryExpAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> nextExp; // Exp or Number
    PrimaryExpAST(std::unique_ptr<BaseAST> &_nextExp)
    {
        nextExp = std::move(_nextExp);
    }
    void *build_koopa_values() const override
    {
        return nextExp->build_koopa_values();
    }
    int CalcValue() const override
    {
        return nextExp->CalcValue();
    }
};

class UnaryExpAST : public BaseAST
{
public:
    enum
    {
        Primary,
        Op,
        Function
    } type;
    std::string op;
    std::unique_ptr<BaseAST> nextExp; // PrimaryExp or UnaryExp
    std::vector<BaseAST*> funcRParams;

    UnaryExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        nextExp = std::move(_primary_exp);
    }
    UnaryExpAST(const char *_op, std::unique_ptr<BaseAST> &_unary_exp)
    {
        type = Op;
        op = std::string(_op);
        nextExp = std::move(_unary_exp);
    }
    UnaryExpAST(const char *_ident, std::vector<BaseAST*> &rparams) : op(_ident), funcRParams(rparams)
    {
        type = Function;
    }

    void *build_koopa_values() const override
    {
        NumberAST zero(0);
        koopa_raw_value_data *res = nullptr;
        koopa_raw_function_data_t *func = nullptr;
        std::vector<const void *> rpa;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)nextExp->build_koopa_values();
            break;
        case Function:
            func = (koopa_raw_function_data_t *)symbol_list.GetSymbol(op).number;
            for(auto rp : funcRParams)
                rpa.push_back(rp->build_koopa_values());
            res = new koopa_raw_value_data();
            res->ty = func->ty->data.function.ret;
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_CALL;
            res->kind.data.call.callee = func;
            res->kind.data.call.args = make_koopa_rs_from_vector(rpa, KOOPA_RSIK_VALUE);
            block_maintainer.AddInst(res);
            break;
        case Op:
            if (op == "+")
            {
                res = (koopa_raw_value_data *)nextExp->build_koopa_values();
                break;
            }
            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_BINARY;
            auto &binary = res->kind.data.binary;
            if (op == "-")
                binary.op = KOOPA_RBO_SUB;
            else if (op == "!")
                binary.op = KOOPA_RBO_EQ;
            binary.lhs = (koopa_raw_value_t)zero.build_koopa_values();
            binary.rhs = (koopa_raw_value_t)nextExp->build_koopa_values();
            block_maintainer.AddInst(res);
            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return nextExp->CalcValue();
        int res = 0;
        if (op == "+")
            res = nextExp->CalcValue();
        else if (op == "-")
            res = -nextExp->CalcValue();
        else if (op == "!")
            res = !nextExp->CalcValue();
        return res;
    }
};

class MulExpAST : public BaseAST
{
public:
    enum
    {
        Primary,
        Op
    } type;
    std::string op;
    std::unique_ptr<BaseAST> leftExp; // may be primary
    std::unique_ptr<BaseAST> rightExp;

    MulExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        leftExp = std::move(_primary_exp);
    }
    MulExpAST(std::unique_ptr<BaseAST> &_left_exp, const char *_op, std::unique_ptr<BaseAST> &_right_exp)
    {
        type = Op;
        leftExp = std::move(_left_exp);
        op = std::string(_op);
        rightExp = std::move(_right_exp);
    }

    void *build_koopa_values() const override
    {
        koopa_raw_value_data *res = nullptr;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)leftExp->build_koopa_values();
            break;
        case Op:
            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_BINARY;
            auto &binary = res->kind.data.binary;
            if (op == "*")
                binary.op = KOOPA_RBO_MUL;
            else if (op == "/")
                binary.op = KOOPA_RBO_DIV;
            else if (op == "%")
                binary.op = KOOPA_RBO_MOD;
            binary.lhs = (koopa_raw_value_t)leftExp->build_koopa_values();
            binary.rhs = (koopa_raw_value_t)rightExp->build_koopa_values();
            block_maintainer.AddInst(res);
            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return leftExp->CalcValue();
        int res = 0;
        if (op == "*")
            res = leftExp->CalcValue() * rightExp->CalcValue();
        else if (op == "/")
            res = leftExp->CalcValue() / rightExp->CalcValue();
        else if (op == "%")
            res = leftExp->CalcValue() % rightExp->CalcValue();
        return res;
    }
};

class AddExpAST : public BaseAST
{
public:
    enum
    {
        Primary,
        Op
    } type;
    std::string op;
    std::unique_ptr<BaseAST> leftExp; // may be primary
    std::unique_ptr<BaseAST> rightExp;

    AddExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        leftExp = std::move(_primary_exp);
    }
    AddExpAST(std::unique_ptr<BaseAST> &_left_exp, const char *_op, std::unique_ptr<BaseAST> &_right_exp)
    {
        type = Op;
        leftExp = std::move(_left_exp);
        op = std::string(_op);
        rightExp = std::move(_right_exp);
    }

    void *build_koopa_values() const override
    {
        koopa_raw_value_data *res = nullptr;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)leftExp->build_koopa_values();
            break;
        case Op:
            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_BINARY;
            auto &binary = res->kind.data.binary;
            if (op == "+")
                binary.op = KOOPA_RBO_ADD;
            else if (op == "-")
                binary.op = KOOPA_RBO_SUB;
            binary.lhs = (koopa_raw_value_t)leftExp->build_koopa_values();
            binary.rhs = (koopa_raw_value_t)rightExp->build_koopa_values();
            block_maintainer.AddInst(res);
            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return leftExp->CalcValue();
        int res = 0;
        if (op == "+")
            res = leftExp->CalcValue() + rightExp->CalcValue();
        else if (op == "-")
            res = leftExp->CalcValue() - rightExp->CalcValue();
        return res;
    }
};

class RelExpAST : public BaseAST
{
public:
    enum
    {
        Primary,
        Op
    } type;
    std::string op;
    std::unique_ptr<BaseAST> leftExp; // may be primary
    std::unique_ptr<BaseAST> rightExp;

    RelExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        leftExp = std::move(_primary_exp);
    }
    RelExpAST(std::unique_ptr<BaseAST> &_left_exp, const char *_op, std::unique_ptr<BaseAST> &_right_exp)
    {
        type = Op;
        leftExp = std::move(_left_exp);
        op = std::string(_op);
        rightExp = std::move(_right_exp);
    }

    void *build_koopa_values() const override
    {
        koopa_raw_value_data *res = nullptr;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)leftExp->build_koopa_values();
            break;
        case Op:
            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_BINARY;
            auto &binary = res->kind.data.binary;
            if (op == "<")
                binary.op = KOOPA_RBO_LT;
            else if (op == "<=")
                binary.op = KOOPA_RBO_LE;
            else if (op == ">")
                binary.op = KOOPA_RBO_GT;
            else if (op == ">=")
                binary.op = KOOPA_RBO_GE;
            binary.lhs = (koopa_raw_value_t)leftExp->build_koopa_values();
            binary.rhs = (koopa_raw_value_t)rightExp->build_koopa_values();
            block_maintainer.AddInst(res);
            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return leftExp->CalcValue();
        int res = 0;
        if (op == "<")
            res = leftExp->CalcValue() < rightExp->CalcValue();
        else if (op == "<=")
            res = leftExp->CalcValue() <= rightExp->CalcValue();
        else if (op == ">")
            res = leftExp->CalcValue() > rightExp->CalcValue();
        else if (op == ">=")
            res = leftExp->CalcValue() >= rightExp->CalcValue();
        return res;
    }
};

class EqExpAST : public BaseAST
{
public:
    enum
    {
        Primary,
        Op
    } type;
    std::string op;
    std::unique_ptr<BaseAST> leftExp; // may be primary
    std::unique_ptr<BaseAST> rightExp;

    EqExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        leftExp = std::move(_primary_exp);
    }
    EqExpAST(std::unique_ptr<BaseAST> &_left_exp, const char *_op, std::unique_ptr<BaseAST> &_right_exp)
    {
        type = Op;
        leftExp = std::move(_left_exp);
        op = std::string(_op);
        rightExp = std::move(_right_exp);
    }

    void *build_koopa_values() const override
    {
        koopa_raw_value_data *res = nullptr;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)leftExp->build_koopa_values();
            break;
        case Op:
            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_BINARY;
            auto &binary = res->kind.data.binary;
            if (op == "==")
                binary.op = KOOPA_RBO_EQ;
            else if (op == "!=")
                binary.op = KOOPA_RBO_NOT_EQ;
            binary.lhs = (koopa_raw_value_t)leftExp->build_koopa_values();
            binary.rhs = (koopa_raw_value_t)rightExp->build_koopa_values();
            block_maintainer.AddInst(res);
            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return leftExp->CalcValue();
        int res = 0;
        if (op == "==")
            res = leftExp->CalcValue() == rightExp->CalcValue();
        else if (op == "!=")
            res = leftExp->CalcValue() != rightExp->CalcValue();
        return res;
    }
};

class LAndExpAST : public BaseAST
{
    koopa_raw_value_data *make_not_eq_koopa(koopa_raw_value_t exp) const
    {
        NumberAST zero(0);
        koopa_raw_value_data *res = new koopa_raw_value_data();
        res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
        res->name = nullptr;
        res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
        res->kind.tag = KOOPA_RVT_BINARY;
        auto &binary = res->kind.data.binary;
        binary.op = KOOPA_RBO_NOT_EQ;
        binary.lhs = exp;
        binary.rhs = (koopa_raw_value_t)zero.build_koopa_values();
        block_maintainer.AddInst(res);
        return res;
    }

public:
    enum
    {
        Primary,
        Op
    } type;
    std::string op;
    std::unique_ptr<BaseAST> leftExp; // may be primary
    std::unique_ptr<BaseAST> rightExp;

    LAndExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        leftExp = std::move(_primary_exp);
    }
    LAndExpAST(std::unique_ptr<BaseAST> &_left_exp, const char *_op, std::unique_ptr<BaseAST> &_right_exp)
    {
        type = Op;
        leftExp = std::move(_left_exp);
        op = std::string(_op);
        rightExp = std::move(_right_exp);
    }

    void *build_koopa_values() const override
    {
        std::unique_ptr<NumberAST> zero(new NumberAST(0));
        koopa_raw_value_data *res = nullptr;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)leftExp->build_koopa_values();
            break;
        case Op:
            koopa_raw_value_data *temp_var = new koopa_raw_value_data();
            temp_var->ty = make_int_pointer_type();
            temp_var->name = new_char_arr("%temp");
            temp_var->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            temp_var->kind.tag = KOOPA_RVT_ALLOC;
            block_maintainer.AddInst(temp_var);
            
            koopa_raw_value_data *temp_store = new koopa_raw_value_data();
            temp_store->ty = simple_koopa_raw_type_kind(KOOPA_RTT_UNIT);
            temp_store->name = nullptr;
            temp_store->used_by = empty_koopa_rs();
            temp_store->kind.tag = KOOPA_RVT_STORE;
            temp_store->kind.data.store.dest = temp_var;
            temp_store->kind.data.store.value = (koopa_raw_value_t)zero->build_koopa_values();
            block_maintainer.AddInst(temp_store);

            koopa_raw_value_data *br = new koopa_raw_value_data();
            br->ty = simple_koopa_raw_type_kind(KOOPA_RTT_UNIT);
            br->name = nullptr;
            br->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            br->kind.tag = KOOPA_RVT_BRANCH;
            br->kind.data.branch.cond = make_not_eq_koopa((koopa_raw_value_t)leftExp->build_koopa_values());
            koopa_raw_basic_block_data_t *true_block = new koopa_raw_basic_block_data_t();
            koopa_raw_basic_block_data_t *end_block = new koopa_raw_basic_block_data_t();
            br->kind.data.branch.true_bb = true_block;
            br->kind.data.branch.false_bb = end_block;
            br->kind.data.branch.true_args = empty_koopa_rs(KOOPA_RSIK_VALUE);
            br->kind.data.branch.false_args = empty_koopa_rs(KOOPA_RSIK_VALUE);
            block_maintainer.AddInst(br);

            true_block->name = new_char_arr("%true");
            true_block->params = empty_koopa_rs(KOOPA_RSIK_VALUE);
            true_block->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            block_maintainer.AddNewBasicBlock(true_block);
            
            koopa_raw_value_data *b_store = new koopa_raw_value_data();
            b_store->ty = simple_koopa_raw_type_kind(KOOPA_RTT_UNIT);
            b_store->name = nullptr;
            b_store->used_by = empty_koopa_rs();
            b_store->kind.tag = KOOPA_RVT_STORE;
            b_store->kind.data.store.dest = temp_var;
            b_store->kind.data.store.value = make_not_eq_koopa((koopa_raw_value_t)rightExp->build_koopa_values());
            block_maintainer.AddInst(b_store);
            block_maintainer.AddInst(JumpInst(end_block));

            end_block->name = new_char_arr("%end");
            end_block->params = empty_koopa_rs(KOOPA_RSIK_VALUE);
            end_block->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            block_maintainer.AddNewBasicBlock(end_block);

            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_LOAD;
            res->kind.data.load.src = temp_var;
            block_maintainer.AddInst(res);

            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return leftExp->CalcValue();
        int res = 0;
        if (op == "&&")
            res = leftExp->CalcValue() && rightExp->CalcValue();
        return res;
    }
};

class LOrExpAST : public BaseAST
{
    koopa_raw_value_data *make_eq_koopa(koopa_raw_value_t exp) const
    {
        NumberAST zero(0);
        koopa_raw_value_data *res = new koopa_raw_value_data();
        res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
        res->name = nullptr;
        res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
        res->kind.tag = KOOPA_RVT_BINARY;
        auto &binary = res->kind.data.binary;
        binary.op = KOOPA_RBO_EQ;
        binary.lhs = exp;
        binary.rhs = (koopa_raw_value_t)zero.build_koopa_values();
        block_maintainer.AddInst(res);
        return res;
    }

    koopa_raw_value_data *make_not_eq_koopa(koopa_raw_value_t exp) const
    {
        NumberAST zero(0);
        koopa_raw_value_data *res = new koopa_raw_value_data();
        res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
        res->name = nullptr;
        res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
        res->kind.tag = KOOPA_RVT_BINARY;
        auto &binary = res->kind.data.binary;
        binary.op = KOOPA_RBO_NOT_EQ;
        binary.lhs = exp;
        binary.rhs = (koopa_raw_value_t)zero.build_koopa_values();
        block_maintainer.AddInst(res);
        return res;
    }

public:
    enum
    {
        Primary,
        Op
    } type;
    std::string op;
    std::unique_ptr<BaseAST> leftExp; // may be primary
    std::unique_ptr<BaseAST> rightExp;

    LOrExpAST(std::unique_ptr<BaseAST> &_primary_exp)
    {
        type = Primary;
        leftExp = std::move(_primary_exp);
    }
    LOrExpAST(std::unique_ptr<BaseAST> &_left_exp, const char *_op, std::unique_ptr<BaseAST> &_right_exp)
    {
        type = Op;
        leftExp = std::move(_left_exp);
        op = std::string(_op);
        rightExp = std::move(_right_exp);
    }

    void *build_koopa_values() const override
    {
        std::unique_ptr<NumberAST> zero(new NumberAST(0));
        std::unique_ptr<NumberAST> one(new NumberAST(1));
        koopa_raw_value_data *res = nullptr;
        switch (type)
        {
        case Primary:
            res = (koopa_raw_value_data *)leftExp->build_koopa_values();
            break;
        case Op:
            koopa_raw_value_data *temp_var = new koopa_raw_value_data();
            temp_var->ty = make_int_pointer_type();
            temp_var->name = new_char_arr("%temp");
            temp_var->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            temp_var->kind.tag = KOOPA_RVT_ALLOC;
            block_maintainer.AddInst(temp_var);
            
            koopa_raw_value_data *temp_store = new koopa_raw_value_data();
            temp_store->ty = simple_koopa_raw_type_kind(KOOPA_RTT_UNIT);
            temp_store->name = nullptr;
            temp_store->used_by = empty_koopa_rs();
            temp_store->kind.tag = KOOPA_RVT_STORE;
            temp_store->kind.data.store.dest = temp_var;
            temp_store->kind.data.store.value = (koopa_raw_value_t)one->build_koopa_values();
            block_maintainer.AddInst(temp_store);

            koopa_raw_value_data *br = new koopa_raw_value_data();
            br->ty = simple_koopa_raw_type_kind(KOOPA_RTT_UNIT);
            br->name = nullptr;
            br->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            br->kind.tag = KOOPA_RVT_BRANCH;
            br->kind.data.branch.cond = make_eq_koopa((koopa_raw_value_t)leftExp->build_koopa_values());
            koopa_raw_basic_block_data_t *true_block = new koopa_raw_basic_block_data_t();
            koopa_raw_basic_block_data_t *end_block = new koopa_raw_basic_block_data_t();
            br->kind.data.branch.true_bb = true_block;
            br->kind.data.branch.false_bb = end_block;
            br->kind.data.branch.true_args = empty_koopa_rs(KOOPA_RSIK_VALUE);
            br->kind.data.branch.false_args = empty_koopa_rs(KOOPA_RSIK_VALUE);
            block_maintainer.AddInst(br);

            true_block->name = new_char_arr("%true");
            true_block->params = empty_koopa_rs(KOOPA_RSIK_VALUE);
            true_block->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            block_maintainer.AddNewBasicBlock(true_block);
            
            koopa_raw_value_data *b_store = new koopa_raw_value_data();
            b_store->ty = simple_koopa_raw_type_kind(KOOPA_RTT_UNIT);
            b_store->name = nullptr;
            b_store->used_by = empty_koopa_rs();
            b_store->kind.tag = KOOPA_RVT_STORE;
            b_store->kind.data.store.dest = temp_var;
            b_store->kind.data.store.value = make_not_eq_koopa((koopa_raw_value_t)rightExp->build_koopa_values());
            block_maintainer.AddInst(b_store);
            block_maintainer.AddInst(JumpInst(end_block));

            end_block->name = new_char_arr("%end");
            end_block->params = empty_koopa_rs(KOOPA_RSIK_VALUE);
            end_block->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            block_maintainer.AddNewBasicBlock(end_block);

            res = new koopa_raw_value_data();
            res->ty = simple_koopa_raw_type_kind(KOOPA_RTT_INT32);
            res->name = nullptr;
            res->used_by = empty_koopa_rs(KOOPA_RSIK_VALUE);
            res->kind.tag = KOOPA_RVT_LOAD;
            res->kind.data.load.src = temp_var;
            block_maintainer.AddInst(res);

            break;
        }
        return res;
    }
    int CalcValue() const override
    {
        if (type == Primary)
            return leftExp->CalcValue();
        int res = 0;
        if (op == "||")
            res = leftExp->CalcValue() || rightExp->CalcValue();
        return res;
    }
};
