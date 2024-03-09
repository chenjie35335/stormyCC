#include "BaseAST.h"

class FuncDefAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::unique_ptr<BaseAST> FuncFParams;
  int type;
  void Dump() const override
  {
    cout << "fun ";
    ScopeLevel++;
    int dep = ScopeLevel;
    auto child = new IdentTableNode();
    child->father = IdentTable;
    child->level = ScopeLevel;
    IdentTable->child = child;
    IdentTable = IdentTable->child;
    if (type == NO_PARA)
      cout << "@" << ident << "()";
    else
    {
      cout << "@" << ident << "(";
      FuncFParams->Dump();
      cout << ")";
    }

    int ret_type = func_type->calc();
    if (!ret_type)
    {
      funcTable.FuncTable.insert(pair<string, bool>(ident, true));
    }
    else
    {
      funcTable.FuncTable.insert(pair<string, bool>(ident, false));
    }
    // IdentTable= IdentTable->father;
    // int tmp = func_type->calc();
    cout << "{" << endl;
    cout << "%"
         << "entry:" << endl;
    // para stmt declare and push into HashTable

    // add sth
    for (auto const &pair : child->VarTable)
    {
      // std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
      cout << "  @" + pair.first << "_" << to_string(dep) << " = "
           << "alloc i32" << endl;
      cout << "  store "
           << "%" + pair.first << "_" << to_string(dep) << ", "
           << "@" + pair.first << "_" << to_string(dep) << endl;
    }

    block->Dump();

    IdentTable = IdentTable->father;
    delete IdentTable->child;
    // delete IdentTable;
    // delete IdentTable->child2;
    // we need so much content
    // assert(0);
    // cout<<"ret right"<<endl;
    // cout<<ret_cnt<<"ret_cnt"<<endl;
    if (ret_type)
    {
      // cerr << "Warning: "
      cout << "\tret" << endl;
    }
    ret_cnt = 0;
    cout << "}" << endl;
  }
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override { return 0; }
};

class FuncTypeAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> flag;
  int type;
  void Dump() const override
  {
  }
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override
  {
    switch (type)
    {
    case (0):
      cout << ": i32 ";
      break;
    case (1):
      // para->Dump();
      break;
    }
    return type;
  }
};

// function params

class FuncFParamsAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> para;
  int type;
  void Dump() const override
  {
    switch (type)
    {
    case (FUNC_SIN):
      para->Dump();
      break;
    case (FUNC_MUL):
      para->Dump();
      break;
    }
  }
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override { return 0; }
};

class SinFuncFParamAST : public BaseAST
{
public:
  std::string ident;
  std::unique_ptr<BaseAST> ParaType;
  void Dump() const override
  {
    // nFuncFParamAS push into table
    cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
    cout << "i32";
    auto &VarTable = IdentTable->VarTable;
    VarTable.insert(pair<string, int>(ident, 0));
    // auto &VarTable = IdentTable->VarTable;
    // VarTable.insert(pair<string,int>(ident,0));

    // IdentTable->child2 = NULL;
    // delete child2;
  }
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override { return 0; }
};

class MulFuncFParamAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> sin_para;
  std::unique_ptr<BaseAST> mul_para;
  void Dump() const override
  {
    sin_para->Dump();
    cout << ", ";
    mul_para->Dump();
  }
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override { return 0; }
};

class ParaTypeAST : public BaseAST
{
public:
  int type;
  void Dump() const override
  {
    // if(type == 1)
    // cout<<"@"<<ident<<": ";
    cout << "i32";
  }
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override { return 0; }
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
  void Dump(int value) const override {}
  void Dump(string &sign) const override {}
  void Dump(string &sign1, string &sign2, string &sign) const override {}
  [[nodiscard]] int calc() const override { return 0; }
};