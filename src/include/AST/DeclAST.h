#include "BaseAST.h"

class DeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> ConstDecl;
    std::unique_ptr<BaseAST> VarDecl;
    std::unique_ptr<BaseAST> arrDef;
    uint32_t type;
    void Dump() const override {
      //cout<< "declinto" << endl; 
        switch(type) {
            case DECLAST_CON: ConstDecl->Dump(); break;
            case DECLAST_VAR: VarDecl->Dump(DECL_LOC); break;
            case DECLAST_ARR: arrDef->Dump(); break;
            default: assert(0);
        }
    }
};

class ConstDeclAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> Btype;
    std::unique_ptr<BaseAST> MulConstDef;
    std::unique_ptr<BaseAST> arrDef;
    int type;
    void Dump() const override {
      if(type){
        arrDef->Dump();
      } else {
        MulConstDef->Dump();
      } 
    }
};

class BtypeAST : public BaseAST {
  public:
    string type;
};
//这里使用mulConstDef递归嵌套的方式实现多个，而且使用vector存储可以提高树的平衡性
class MulConstDefAST : public BaseAST {
  public:
    vector <unique_ptr<BaseAST>> SinConstDef;
    void Dump() const override{
        for(auto &sinConstDef : SinConstDef) {
          sinConstDef->Dump();
        }
    }
};

class SinConstDefAST : public BaseAST{
  public:
    string ident;
    unique_ptr<BaseAST>ConstExp;
    unique_ptr<BaseAST> arrDef;
    int type;
    unique_ptr<BaseAST> def;
    void Dump() const override{
      //cout<< "sinconstdef" << endl;
      if(type == 0){
        auto &ValueTable = IdentTable->ConstTable;
        auto &VarTable   = IdentTable->VarTable;
        if(ValueTable.find(ident) != ValueTable.end()) {
          assert(0);
        } 
        int value = ConstExp->calc();
        ValueTable.insert(pair<string,int>(ident,value));
      }else {
        def->Dump();
      }
      
    }
};

class VarDeclAST : public BaseAST {
public:
     unique_ptr <BaseAST> MulVarDef;
     void Dump(int sign) const override{
        MulVarDef->Dump(sign);
     }
};

class MulVarDefAST : public BaseAST {
public:
    vector <unique_ptr<BaseAST>> SinValDef;
    void Dump(int sign) const override{
      for(auto &sinValDef : SinValDef) {
          sinValDef->Dump(sign);
      }
    }
};

class SinVarDefAST : public BaseAST {
public:
    string ident;
    unique_ptr<BaseAST>InitVal;
    unique_ptr<BaseAST>func_exp;
    unique_ptr<BaseAST>ConstInit;
    unique_ptr<BaseAST>dimen;
    int number;
    uint32_t type;
    void Dump(int sign) const override{
      //cout<< "sinconstdef" << endl;
      arrayTable.ident = ident;
      int value;
      auto &ValueTable = IdentTable->ConstTable;
      auto &VarTable = IdentTable->VarTable;
      int dep = IdentTable->level;
      if(ValueTable.find(ident) != ValueTable.end()){
          cerr << '"' << ident << '"' << " has been defined as constant" <<endl;
          exit(-1);
      } 
       if(VarTable.find(ident) != VarTable.end()){
          cerr << '"' << ident << '"' << " redefined" <<endl;
          exit(-1);
        }
      string sign1;
      switch(type) {
        //we need add type
        case SINVARDEFAST_UIN: {
          if(sign == DECL_GLOB) 
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc i32," <<  " zeroinit" << endl;
          else {
            cout << "  @"<<ident+"_"+to_string(dep) <<" = " << "alloc i32" << endl;
          }
          break;
        }
        case SINVARDEFAST_INI:
        {
          if(sign == DECL_GLOB) {
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc i32," <<  " "<<InitVal->calc() << endl;  
            cout<<endl;
          }else {
            cout << "  @"+ident+"_"+to_string(dep) <<" = " << "alloc i32" << endl;
            InitVal->Dump(sign1);
            cout << "  store " << sign1<< ", " << "@"+ident+"_"+to_string(dep)<<endl;
          }
          break;
        }
        case SINVARDEFAST_UNI_ARR:
        {
          string sign2;
          vector<string> Para;
          dimen->Dump(sign2,Para);
          string res;

          for(auto it = Para.rbegin(); it < Para.rend(); ++it){
            //cout<<"520";
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
          //cout<<res<<" ";

          if(sign == DECL_GLOB){
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc " << res <<"," <<" zeroinit" << endl;
            cout<<endl;
          } else {
            cout<< "  @" + ident +"_" + to_string(dep) <<" = " << "alloc " << res <<endl;
          }
          break;
        }
        case SINVARDEFAST_INI_ARR:
        {
          string sign3,sign4;
          vector<string> Para;
          dimen->Dump(sign3,Para);
          vector<string> Para1 = Para;
          string res;
          for(auto it = Para.rbegin(); it < Para.rend(); ++it){
            //cout<<"520";
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
          //cout<<res<<" ";
          Para.clear();
          //ConstInit->Dump(sign4,Para);
          if(sign == DECL_GLOB) {
            cout << "global @"+ident+"_"+to_string(dep) <<" = " << "alloc " << res <<endl; 
            ConstInit->Dump(sign4,Para1);
            cout<<endl;
          }else {
            cout << "  @"+ident+"_"+to_string(dep) <<" = " << "alloc "<< res << endl;
            //ConstInit->Dump(sign1);
            //cout << "  store " << sign4<< ", " << "@"+ident+"_"+to_string(dep)<<endl;
            string sign5 = ident;
            //cout<<sign5;
            ConstInit->Dump(sign5,Para1);
            //cout<<"666";
          }
          break;
        }
        case SINVARDEFAST_ASSIGNMENT:
        {
          string sign5;
          vector<string> PARA3;
          dimen->Dump(sign5,PARA3);
          string s0 = "@" + ident;
          for(auto it = PARA3.begin(); it != PARA3.end(); it++){
            cout<<"  %"<<alloc_now << " = "<<"getelemptr " <<  s0 << *it<<endl;
            s0 = "%" + to_string(alloc_now); 
            alloc_now++;
          }
          cout<<"  store "<< number << ", "<<"%" <<alloc_now<<endl;
          break;
        }
        default: assert(0);
      }
      VarTable.insert(pair<string,int>(ident,value));
      arrayTable.ident = "";
    }
};

class InitValAST : public BaseAST {
public:
    unique_ptr<BaseAST>Exp;
    void Dump(string & sign) const override {
      Exp->Dump(sign);
    }
    [[nodiscard]] int calc() const override {return Exp->calc();}
};

class ConstExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST>Exp;
    [[nodiscard]] int calc() const override{
        return Exp->calc();
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
