#include "BaseAST.h"
#include <string>
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
        
        cout<<res;
        string sign1 = ident;
        arrayTable.ident = sign1;
        arrayVar->Dump(sign1,Para);
        arrayTable.ident = "";
        //cout<<sign1<<endl;
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
        int dep = IdentTable->level;
        if(type ==0){
          //fill with zero
          vector<string> Para1 = Para;
          Para.clear();
          content->Dump(sign,Para);
          //all output here filling-zero alorithm
          // ArrayTable ---  Para  Compare !!!
          int total = 1;
          for(auto it = Para1.begin(); it != Para1.end(); it++){
            total*=stoi(*it);
          }
          if(total>200) {
              cout<<total;
              total = 200;
          }
          //memery emit
          //simplify only for three level
          int k = 0;
          auto it = Para1.begin();

          if(alloc_now < 0) alloc_now = 0;
          //for(auto it = Para.begin(); it != Para.end(); it++){
            for(int i = 0; i < total ; i++){
              //add break condition
              if(arrayTable.ArrLevel[i] != -1 || ((i+1)%stoi(*it) ) == 0){
                cout<<endl;
                cout<<"  %"<<alloc_now<<" = getelemptr @"<<arrayTable.ident<<"_"<<dep<<", "<<i<<endl;
                cout<<"  store "<<arrayTable.ArrTable[k]<<", %"<<alloc_now<<endl;
                alloc_now++;
                k++;
                //if(k == arrayTable.ArrTable.size()) break;
              } else {
                cout<<endl;
                cout<<"  %"<<alloc_now<<" = getelemptr @"<<arrayTable.ident<<"_"<<dep<<", "<<i<<endl;
                cout<<"  store "<<"0"<<", %"<<alloc_now<<endl;
                alloc_now++;
              }
            }
          //}
          arrayTable.ArrTable.clear();
          arrayTable.ArrLevel.clear();
          arr_seq_cnt = 0;
        } else {
          sign = "";
          sign.append("{0}");
          cout<<endl;
        }
    }
};



class ArrayContentAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> var;
    int type;
    //three situation
    void Dump(string &sign, vector<string> &Para) const override{
        //this Para contain dimension
        if(type == 0){
          arrayTable.ArrTable[arr_seq_cnt] = var->calc();
          arrayTable.ArrLevel[arr_seq_cnt] = arr_dep;
          arr_seq_cnt++;

        } else if(type == 1){
          var->Dump(sign,Para);
          
        } else if(type == 2){
          arr_dep++;
          var->Dump(sign,Para);
          arrayTable.ArrLevel[arr_seq_cnt] = -1;
          arr_dep--;
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
        arrayTable.ArrTable[arr_seq_cnt] = sin->calc();
        //ArrayTable.ArrTable.push_back(arr_seq_cnt,var->calc());
        arr_seq_cnt++;
        Para.push_back(sign);
        mul->Dump(sign,Para);
      } else {
        vector<string> Para1;
        arr_dep++;
        sin->Dump(sign,Para1);
        arrayTable.ArrLevel[arr_seq_cnt] = -1;
        arr_dep--;
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
        //cout<<"777";
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
        auto s1 = Para.begin();
        sign = "@" + ident;
        //sign.append(to_string(s1));
        //cout<<"\t%"<<alloc_now<<" = "<<"load "<<s0<<endl;     
    }
     [[nodiscard]] int calc() const override {return 38;}
};

//arra para
class ArrParaAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    int type;
    void Dump(string &sign,vector<string> &Para) const override{
      if(type == 0){
        exp->Dump(sign);
        Para.push_back(sign);
      }else {
        exp->Dump(sign,Para);
      }
    }
};

class MulArrParaAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> para;
    void Dump(string &sign,vector<string> &Para) const override {
      exp->Dump(sign);
      Para.push_back(sign);
      para->Dump(sign,Para);
    }
};



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