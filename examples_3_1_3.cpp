#include <iostream>
#include <vector>

using namespace std;

/*
3.1.3. Unimform Initializtion and Initializer lists
*/

void print(initializer_list<int> vals)
{
    for (auto p = vals.begin(); p != vals.end(); p++) { // process a list of values
        cout << *p << ' ';
    }
    cout << endl;
}

class P
{
public:
    P(int, int) {}
    P(initializer_list<int>) {}
};

int main()
{
    {
    int i;      // i has undefined value
    int j{};    // j is initialized by 0
    int* p;     // p has undefined value
    int* q{};   // q is initialized by nullptr

    cout << "i = " << i << '\n';    // warning C4700: uninitialized local variable 'i' used
    cout << "j = " << j << '\n';
    cout << "p = " << p << '\n';    // warning C4700: uninitialized local variable 'p' used
    cout << "q = " << q << '\n';
    }
    /*
    i = 15365038        // undefined value
    j = 0
    p = 012FF970        // undefined value
    q = 00000000
    */

   {
    int x1(5.3);        // OK, but x1 = 5 (narrowing conversiont)
    int x2 = 5.3;       // OK, but x2 = 5
    //int x3{5.0};      // error: narrowing initialization not OK
    //int x4 = {5.3};   // same
    char c1{7};         // OK: no narrowing conversion
    //char c2{999999};  // Error: narrowing initialization not OK.
    vector<int> v1 {1, 2, 4, 5};    // OK
    //vector<int> v2 {1, 2.3, 4, 5.6};  // Error: narrowing doubles to ints - not OK with initialization

    cout << "x1 = " << x1 << '\n';
    cout << "x2 = " << x2 << '\n';
    cout << "c1 = " << c1 << '\n';
   }
    /*
    x1 = 5      // value narrowed
    x2 = 5      // value narrowed
    c1 =        // integer value 7 is converted to a char, which is not a printable
    */

    print({12, 3, 5, 7, 11, 13, 17});   // pass a list of values to print()

    /*
    When there are constructors for both a specific number of arguments and an initializer list, the version with the initializer list is prefered.
    */
   {
    P p(77, 5);         // calls P::P(int, int)
    P q{77, 5};         // calls P::P(initializer_list)
    P r{77, 5, 42};     // calls P::P(initializer_list)
    P s = {77, 5};      // calls P::P(initializer_list)
   }

    return 0;
}


