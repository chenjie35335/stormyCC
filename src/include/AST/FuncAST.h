#include "BaseAST.h"

class FuncDefAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::unique_ptr<BaseAST> FuncFParams;
  void Dump() const override
  {
    alloc_now = -1;
    cout << "fun ";
    ScopeLevel++;
    int dep = ScopeLevel;
    auto child = new IdentTableNode();
    child->father = IdentTable;
    child->level = ScopeLevel;
    IdentTable->child = child;
    IdentTable = IdentTable->child;
    cout << "@" << ident << "(";
    FuncFParams->Dump();
    cout << ")";
    IdentTable = IdentTable->father;
    int ret_type = func_type->calc();
    if (ret_type == FUNCTYPE_INT)
      funcTable.FuncTable.insert(pair<string, bool>(ident, true));
    else
      funcTable.FuncTable.insert(pair<string, bool>(ident, false));
    cout << "{" << endl;
    cout << "%"
         << "entry:" << endl;
    for (auto const &pair : child->VarTable)
    {
      cout << "  @" + pair.first << "_" << to_string(dep) << " = "
           << "alloc i32" << endl;
      cout << "  store "
           << "%" + pair.first << "_" << to_string(dep) << ", "
           << "@" + pair.first << "_" << to_string(dep) << endl;
    }
    block->Dump();
    if (ret_type == FUNCTYPE_VOID)
    {
      cout << "\tret" << endl;
    }
    ret_cnt = 0;
    cout << "}" << endl << endl;
  }
};

class FuncTypeAST : public BaseAST
{
public:
  int type;
  [[nodiscard]] int calc() const override
  {
    switch (type)
    {
    case FUNCTYPE_INT:
      cout << ": i32 ";
      break;
    case FUNCTYPE_VOID:
      break;
    default:
      assert(0);
    }
    return type;
  }
};

// function params

class FuncFParamsAST : public BaseAST
{
public:
  vector<unique_ptr<BaseAST>> para;
  void Dump() const override
  {
       for(auto it=para.begin();it!=para.end();it++)
                {
                    if(it!=para.begin()) std::cout<<',';
                    (*it)->Dump();
                }
  }
};

class SinFuncFParamAST : public BaseAST
{
public:
  std::string ident;
  std::unique_ptr<BaseAST> ParaType;
  void Dump() const override
  {
    cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
    cout << "i32";
    auto &VarTable = IdentTable->VarTable;
    VarTable.insert(pair<string, int>(ident, 0));
  }
};

class ParaTypeAST : public BaseAST
{
public:
  void Dump() const override
  {
    cout << "i32";
  }
};

// global vare
class GlobalDeclAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> global;
    // std::unique_ptr<BaseAST> mul;
    void Dump() const override
    {
      cout << "global"
          << " ";
      global->Dump();
      cout << endl;
    } 
};