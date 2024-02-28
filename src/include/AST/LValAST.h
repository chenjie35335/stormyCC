#include "BaseAST.h"

// class LValAST : public BaseAST {
//     public:
//       string ident;
//       //int value;
//       void Dump() const override {
//         cout << "@" <<ident << endl;
//       }
//       void Dump(int value) const override {
//         cout << "@" <<ident << endl;
//         VarTable[ident] = value;
//       }
//       void Dump(string &sign) const override {
//        if( ValueTable.find(ident) != ValueTable.end()){
//           int CalValue = ValueTable.at(ident);
//           sign = to_string(CalValue);
//        }
//        else if(VarTable.find(ident) != VarTable.end()) {
//           alloc_now++;
//           sign = "%"+to_string(alloc_now);
//           cout << "  "<<sign << " = " << "load " << "@"+ident<<endl;
//        }
//        else {
//          cerr << "Error: " << '"' << ident << '"' << " is not defined" << endl;
//        }
          
//       }
//       void Dump(string &sign1,string &sign2,string &sign3) const override{}
//       [[nodiscard]] int calc() const override{
//         if (ValueTable.find(ident) != ValueTable.end())
//         {
//           int CalValue = ValueTable.at(ident);
//           return CalValue;
//         }
//         else if(VarTable.find(ident) != VarTable.end()) {
//           return VarTable.at(ident);
//         }
//         else
//         {
//           cerr << "Error: " << '"' << ident << '"' << "is not defined" << endl;
//           exit(-1);
//         }
//       }
// };

class LValLAST : public BaseAST {
  public:
    string ident;
    void Dump() const override {}
    void Dump(string &sign) const override {
      sign = ident;
    }
    void Dump(string &sign, string &sign1, string &sign2) const override {}
    void Dump(int value) const override {}  
    int calc() const override {return 0; }
};

class LValRAST : public BaseAST {
  public:
    string ident;
    void Dump() const override {}
    void Dump(string &sign) const override {
      if( ValueTable.find(ident) != ValueTable.end()){
          int CalValue = ValueTable.at(ident);
          sign = to_string(CalValue);
       }
       else if(VarTable.find(ident) != VarTable.end()) {
          alloc_now++;
          sign = "%"+to_string(alloc_now);
          cout << "  "<<sign << " = " << "load " << "@"+ident<<endl;
       }
       else {
         cerr << "Error: " << '"' << ident << '"' << " is not defined" << endl;
       }
    }
    void Dump(string &sign, string &sign1, string &sign2) const override {}
    void Dump(int value) const override {}  
    int calc() const override {
      if (ValueTable.find(ident) != ValueTable.end())
        {
          int CalValue = ValueTable.at(ident);
          return CalValue;
        }
        else if(VarTable.find(ident) != VarTable.end()) {
          return VarTable.at(ident);
        }
        else
        {
          cerr << "Error: " << '"' << ident << '"' << "is not defined" << endl;
          exit(-1);
        } 
      }
};