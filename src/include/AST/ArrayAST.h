#include "BaseAST.h"

class ConstArrayDefAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> dimension;
    std::unique_ptr<BaseAST> arrayVar;
    string ident;
    void Dump() const override{
        //deal with res
        int dep = IdentTable->level;
        cout<<"  @"<<ident<<"_"<<dep<<" = ";
        string sign;
        vector<string> Para;
        dimension->Dump(sign,Para);
        //cout<<",";
        cout<<"alloc ";
        string res;

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
        cout<<res<<", ";

        string sign1;
        vector<string> Para1;
        arrayVar->Dump(sign1,Para1);
        cout<<sign1<<endl;

    }
};


class ArrayDimenAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> constExp;
    int type;
    void Dump(string &sign,vector<string> &Para) const override{
      if(type == 0){
        //cout<<"666";
        int tmp = constExp->calc();
        sign = to_string(tmp);
        Para.push_back(sign);
      }else {
        constExp->Dump(sign,Para);
      }
    }
};

class MulArrayDimenAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> dim;
    void Dump(string &sign,vector<string> &Para) const override {
      int tmp = exp->calc();
      sign = to_string(tmp);
      Para.push_back(sign);
      dim->Dump(sign,Para);
    }
};

class ConstArrayVarAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> content;
    int type;
    void Dump(string &sign, vector<string> &Para) const override{
        if(type ==0){
          //fill with zero
          content->Dump(sign,Para);
          sign.insert(0,"{");
          sign.append("}");
        } else {
          sign = "";
          sign.append("{0}");
        }
    }
};

class ArrayContentAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var;
    int type;
    //three situation
    void Dump(string &sign, vector<string> &Para) const override{
        if(type == 0){
          sign = to_string(var->calc());
          Para.push_back(sign);
        } else if(type == 1){
          var->Dump(sign,Para);
          string sign1 = "";
          for(auto it = Para.begin(); it != Para.end(); it++){
            sign1.append(*it);
            if(it != Para.end() - 1) {
              sign1.append(", ");
            } 
          }
          sign = sign1;
        } else if(type == 2){
          var->Dump(sign,Para);
        } 
    }
};

class MulArrayContentAST : public BaseAST {
  public: 
    std::unique_ptr<BaseAST> sin;
    std::unique_ptr<BaseAST> mul;
    int type;
    void Dump(string &sign,vector<string> &Para) const override {
      if(type == 0){
        sign = to_string(sin->calc());
        Para.push_back(sign);
        mul->Dump(sign,Para);
      } else {
        vector<string> Para1;
        sin->Dump(sign,Para1);
        Para.push_back(sign);
        mul->Dump(sign,Para);
      }
    }
};

class MulLValLAST : public BaseAST {
  public:
    std::string ident;
    std::unique_ptr<BaseAST> exp;
    void Dump(string &sign) const override {
        //cout<<"666";
        vector<string> Para;
        exp->Dump(sign,Para);
        if(alloc_now<0) alloc_now = 0;
        int dep = IdentTable->level;
        string s0 = "@";
        s0.append(ident);
        s0.append("_"+to_string(dep));
        for(auto it = Para.begin(); it !=Para.end() ; it++){
          cout<<"\t%ptr_"<<alloc_now <<" = getelemptr "<< s0;
          cout<<", "<<*it<<endl;
          s0 = "%ptr_";
          s0.append(to_string(alloc_now));
          alloc_now++;
        }
        cout<<"\t%"<<alloc_now<<" = "<<"load "<<s0<<endl;     
    }
};

//arra para




//1. int a[100];
//a[100] = {0};
//define a[exp]  b[var]
//2. 2nd dimension
// a[1][5] define;
//3.  global array define
// int // const int 
//way:
//4. giving value to the array: 
// {1,2,3,}
// auto fill it with 0
//5. 2nd dimension array giving value
//using it in global