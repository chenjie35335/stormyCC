#include "BaseAST.h"

class FuncDefAST : public BaseAST
{
public:
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::unique_ptr<BaseAST> FuncFParams;
  void Dump(int sign) const override
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
    int ret_type = sign;
    if (ret_type == FUNCTYPE_INT){
      cout << ": i32 ";
      funcTable.FuncTable.insert(pair<string, bool>(ident, true));
    }
    else
      funcTable.FuncTable.insert(pair<string, bool>(ident, false));
    cout << "{" << endl;
    cout << "%"
         << "entry:" << endl;
    for (auto const &pair : child->VarTable)
    {
      if(pair.second == 0){
        cout << "  @" + pair.first << "_" << to_string(dep) << " = "
           << "alloc i32" << endl;
        cout << "  store "
           << "%" + pair.first << "_" << to_string(dep) << ", "
           << "@" + pair.first << "_" << to_string(dep) << endl;
      } else {
        cout << "  @" + pair.first << "_" << to_string(dep) << " = "
           << "alloc *i32" << endl;
        cout << "  store "
           << "%" + pair.first << "_" << to_string(dep) << ", "
           << "@" + pair.first << "_" << to_string(dep) << endl;
      } 
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
  std::unique_ptr<BaseAST> Dimen;
  int type;
  void Dump() const override
  {
    switch(type){
      case PARA_VAR:{
        cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
        cout << "i32";
        auto &VarTable = IdentTable->VarTable;
        VarTable.insert(pair<string, int>(ident, 0));
        break;
      } 
      case PARA_ARR_SIN:{
        cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
        cout<< "*i32";
        auto &VarTable = IdentTable->VarTable;
        VarTable.insert(pair<string, int>(ident, -1));
        break;
      }
      case PARA_ARR_MUL:{
        cout << "%" << ident << "_" << to_string(ScopeLevel) << ": ";
        cout<< "*";
        string res,sign;
        vector<string> Para;
        Dimen->Dump(sign,Para);
        //cout<<sign;
         for(auto it = Para.rbegin(); it < Para.rend(); ++it){
          if(it == Para.rbegin()){
            res.append("[");
            res.append("i32");
            res.append(", ");
            res.append(*it);
            res.append("]");
          } else {
            res.insert(0,"[");
            res.append(", ");
            res.append(*it);
            res.append("]");
          }
        }
        cout<<res;
        auto &VarTable = IdentTable->VarTable;
        VarTable.insert(pair<string, int>(ident, -2));
        break;
      }
        break;
    }
    
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

