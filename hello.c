int main() {
    /*zhuzhu*/
    //return !(- -!6); //woshizhu
    //return 0;
    //return (1+2*(3+4)/5);
    //return (1+2*(3+4)/5) && 1 <= 3 || 2 >= 4 || !(- -!6);
    //const int x = 1+1,z = !(- -!6);
    //const int y = (1+2*(3+4)/5) && 1 <= 3 || 2 >= 4 || !(- -!6);
    //return x+y/z*4;
    // int x;
    // x = 1;
    // int a = 1;
  // {
    // a = 2;
    // int a = 3;
  // }
  // return a;
  // const int x = 1;
  // return x;
  // int a = 1;
  // {
    // a = a + 2;
    // int a = 3;
    // a = a + 4;
  // }
  // a = a + 5;
  // return a;
  int a = 1, sum = 2;
  {
    a = a + 2;
    int b = a + 3;
    b = b + 4;
    sum = sum + a + b;
    {
      b = b + 5;
      int c = b + 6;
      a = a + c;
      sum = sum + a + b + c;
      {
        b = b + a;
        int a = c + 7;
        a = a + 8;
        sum = sum + a + b + c;
        {
          b = b + a;
          int b = c + 9;
          a = a + 10;
          const int a = 11;
          b = b + 12;
          sum = sum + a + b + c;
          {
            c = c + b;
            int c = b + 13;
            c = c + a;
            sum = sum + a + b + c;
          }
          sum = sum - c;
        }
        sum = sum - b;
      }
      sum = sum - a;
    }
  }
  return sum % 77;
    // return x;
   //int x;
   //const int k = 10 + 11;//k = 21
   //int y = k;//y = 21
   //x = y + 1;//x = 22;
   //const int n = k * 7;//21*7==147;
   //int z = n - x, w = n - y;//z = 147-22=125 w = 147-21=126
   //w = w * 1 * 1 * 1;//w = 126
   //return z + w;//125+126=251
    //return !0;
}