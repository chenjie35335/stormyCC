#include "BaseAST.h"

class LValLAST : public BaseAST {
  public:
    string ident;
    int type;
    std::unique_ptr<BaseAST> array;
    void Dump(string &sign) const override {
      if(type == 0)
      sign = ident;
      else {
        //cout<<"666";
        array->Dump(sign);
      }
    }
    [[nodiscard]] int calc() const override {return array->calc();}
};

class LValRAST : public BaseAST {
  public:
    string ident;
    std::unique_ptr<BaseAST> array;
    int type;
    void Dump() const override {}
    void Dump(string &sign) const override {
      int dep = IdentTable->level;
      //cout<<"666";
      vector<string> Para;
      string s0 = "@";
      s0.append(ident);
      s0.append("_"+to_string(dep));
      if(type == 1){
        array->Dump(sign,Para);
        for(auto it = Para.begin(); it !=Para.end() ; it++){
          cout<<"\t%ptr_"<<alloc_now <<" = getelemptr "<< s0;
          cout<<", "<<*it<<endl;
          s0 = "%ptr_";
          s0.append(to_string(alloc_now));
          alloc_now++;
        }
        return;
      }
      auto p = IdentTable;
      while(p != nullptr) {
      auto &ValueTable = p->ConstTable;
      auto &VarTable   = p->VarTable;
      int dep = p->level;
      //cout << "value = " << VarTable.at(ident) << endl;
      if( ValueTable.find(ident) != ValueTable.end()){
          int CalValue = ValueTable.at(ident);
          sign = to_string(CalValue);
          break;
       }
       else if(VarTable.find(ident) != VarTable.end()) {
          alloc_now++;
          sign = "%"+to_string(alloc_now);
          cout << "  "<<sign << " = " << "load " << "@"+ident+"_"+to_string(dep)<<endl;
          //is_lva = 1;
          break;
       }
       p = p->father;
      }
      //cout << 1 << endl;
       if(p == nullptr) {
         cerr << "Error: " << '"' << ident << '"' << " is not defined" << endl;
         exit(-1);
       }
    }
    int calc() const override {
      auto p           = IdentTable;
      while(p != nullptr) {
      auto &ValueTable = p->ConstTable;
      auto &VarTable   = p->VarTable;
      if (ValueTable.find(ident) != ValueTable.end())
        {
          int CalValue = ValueTable.at(ident);
          return CalValue;
        }
        else if(VarTable.find(ident) != VarTable.end()) {
          return VarTable.at(ident);
        }
        p = p->father;
      }
        if(p == nullptr)
        {
          cerr << "Error: " << '"' << ident << '"' << "is not defined" << endl;
          exit(-1);
        } 
      }
};