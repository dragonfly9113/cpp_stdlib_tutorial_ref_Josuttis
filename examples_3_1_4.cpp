/*
3.1.4 Ranged-based for loops
*/
#include <iostream>
#include <vector>

using namespace std;

/*
to avoid calling the copy constructor and the destructor for each element, you should usually declare the current element to be a constant reference. thus, a generic function to print all elements of a collection should be implemented as follows.
*/
template<typename T>
void printElements(const T& coll)
{
    for (const auto& elem : coll)   // declare the current element to be a constant reference to avoid calling the copy constructor and destructor for each element.
        cout << elem << ' ';
    cout << endl;
}

/*
If we don't want to use range-based for loop, we can use a traditional for loop as below. In comparison, a range-based for loop is much simpler.
*/
template<typename T>
void printElements1(const T& coll)
{
    for (auto _pos = coll.begin(); _pos != coll.end(); ++_pos) {
        const auto& elem = *_pos;
        cout << elem << ' ';
    }
    cout << endl;
}

/*
Or if T doesn't have members being() and end(), we can use std::begin() and std::end() taking coll as argument.
*/
template<typename T>
void printElements2(const T& coll)
{
    for (auto _pos = begin(coll), _end = end(coll); _pos != _end; ++_pos) {
        const auto& elem = *_pos;
        cout << elem << ' ';
    }
    cout << endl;
}

class C
{
public:
    explicit C(const string& s) {}
};

int main()
{
    for (int i : {2,3,5,7,9,13,17,19}) {
        cout << i << ' ';
    }
    cout << endl;

    /*
    declaring elem as a reference is important because otherwise the statements in the body of the for loop act on a local copy of the elements in the vector.
    */
   {
    vector<double> vec {1.0, 2.1, 3.2, 4.3, 5.4, 6.5};
    for (auto& elem : vec) {
        elem *= 3;
    }
    for (auto& elem : vec) {
        cout << elem << ' ';
    }
    cout << endl;

    vector<int> vec1{1,2,3,4};
    vector<double> vec2{1.1, 2.2, 3.3, 4.4};
    printElements(vec1);
    printElements(vec2);
    printElements1(vec1);
    printElements1(vec2);
    printElements2(vec1);
    printElements2(vec2);
   }

    /*
    You can use range-based for loop even for initializer list because the class template std::initializer_list<> provides begin() and end() members.
    */
    for (auto elem : {11, 22, 33, 44})
        cout << elem << ' ';
    cout << endl;

    /*
    No explicit type conversion are possible when elements are initialized as decl inside the for loop.
    */
    vector<string> vs {"hello", "world", "good", "bad"};
    C c{vs[0]};
    //for (const C& elem : vs)      // Error: no explicity type conversion are possible when elements are initialized as decl inside the for loop.
    //    cout << elem << ' ';
    cout << endl;

    return 0;
}



