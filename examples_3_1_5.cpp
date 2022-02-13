/*
3.1.5 Move semantics and rvalue references
*/
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class X
{
public:
    X() {}
    X(const X& lvalue) {}
    X(X&& rvalue) {}
};

void createAndInsert(multiset<string>& coll)
{
    /*
    Here, we insert a new object into a collection, which provides a member function that creates an internal copy of the passed element:
        template <bool _Multi2 = _Multi, enable_if_t<_Multi2, int> = 0>
        iterator insert(const value_type& _Val) {
            return iterator(_Emplace(_Val).first, _Get_scary());
        }
    This behavior is useful because the collection provides value semantics and the ability to insert temporary objects or objects that are used and modified after being inserted.
    In the below example, after being inserted into coll, s (an object of string) can still be used and modified. This is possible because the insert() version called here makes an internal copy of the passed element: s.
    */
    string s{"Hello"};
    coll.insert(s);
 
    /*
    The result of s + s is a temporary rvalue which is no longer used. Since C++11, the temporary rvalue (result of s + s) will most likely be moved into the collection (instead of copied).
    This seems to what's happening here. The move version of insert() is called:
        template <bool _Multi2 = _Multi, enable_if_t<_Multi2, int> = 0>
        iterator insert(value_type&& _Val) {
            return iterator(_Emplace(_STD move(_Val)).first, _Get_scary());
        }
    So the compiler is smart to use the move version of insert() because it knows that the result of s + s is a temporary rvalue and will no longer be used.
    */
    coll.insert(s + s);
    
    /*
    Here again, the copy version of intert() is called because s is a lvalue and the compiler is not sure if it will be used later or not so it chooses the safer copy version of insert().
    */
    coll.insert(s);
    
    /*
    With std::move(), declared in <utility>, s can be moved instead of being copied. However, std::move() doesn't itself do any moving, but merely converts its argument into a so-called rvalue reference, which is a type decalred with two ampersands: string&&. The new type stands for rvalues (anonymous temporaries that can appear only on the right-hand side of an assignment) that can be modified. The contract is that this is a (temporary) object that is not needed any longer so that you can steal its contents and/or its resources.
    First the argument is forwarded as movable:
        template <class _Ty>
        _NODISCARD constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept { // forward _Arg as movable
            return static_cast<remove_reference_t<_Ty>&&>(_Arg);
        }
    The move version of insert() is called:
        template <bool _Multi2 = _Multi, enable_if_t<_Multi2, int> = 0>
        iterator insert(value_type&& _Val) {
            return iterator(_Emplace(_STD move(_Val)).first, _Get_scary());
        }
    Finally, descructor of string is called to release its resource. Therefore, after the below insert() call, s as an object is cleared of its content and released of its resource.
    */
    coll.insert(move(s));

    /*
    More details about the move semantics descripbed above. Because of the use of std::move(), the compiler knows that it should choose the version for rvalue refences. The version for rvalue references can now be optimized so that its implementation steals the contents of s. To initialize the new internal elements, we simply call a so-called move constructor of class string, which steals the value of the passed argument to initialize a new object. For example, the move constructor for strings typically just assigns the existing internal character array to the new object instead of createing a new array and copying all elements.
    In addition, you have to ensure that any modification - especailly a desctruction - of the passed object, where the value was stolen from, doesn't impact the state of the new object that now owns the value. Thus you usually have to clear the contents of the passed argument. So we can see that after the above insert(move(s)), the contents of s is cleared out.
    In fact, for the classes of the C++ stdlib in general, it is guaranteed that after a move, the objects are in a valid but unspecified state.
    */
}

/*
If we implement our own class X. We should implement both copy constructor and move constructor in order to take advantage of the move semantics. In the same way, any nontrivial class should provide both a copy assignment and a move assignment operator.
For strings and collections these operations could be implemented by simply swapping the internal contents and resources. However, you should also clear the contents of *this because this object might hold resources, such as locks, for which it is better to release them sooner. Again, the move semantics don't require that, but it is a quality of move support that, for example, is provided by the container classes of the C++ stdlib.
*/
void createAndInsert1(vector<X>& coll)
{
    X x;
    coll.push_back(move(x));
}

void foo(const string&) {}
void foo(string&&) {}

/*
Returning rvalue reference
You don't have to and should not move() return values. 
The standard specified that for the below code:
* If X (string) has an accessible copy or move constructor, the compiler may choose elide the copy. This is the so-called (named) return value optimization ((N)RVO).
* Otherwise, if X has a move constructor, x is moved.
* Otherwise, if X has a copy constructor, x is copied.
* Otherwise, a compile-time error is emitted.
*/
string foo1()
{
    string s {"Hello"};
    return s;
}

/*
Returning an rvalue reference is an error if the returned object is a local nonstatic object. Thus the below code will NOT compile!
An rvalue reference is a reference, and returning it while referring to a local object means that you return a reference to an object that doesn't exist any more. Whether std::move() is used doesn't matter.
*/
#if 0
string&& foo1()
{
    string s {"Hello"};
    return s;
}
#endif

int main()
{
    multiset<string> ms {};
    vector<X> vx {};

    createAndInsert(ms);
    createAndInsert1(vx);

    /*
    Overloading rules for rvalue an lvalue references
    What I observed is a little different from the book. The behaviour might have changed a bit in modern C++.
    If we implement only
        void foo(X&)  OR  void foo(const X&)
    without void foo(X&&). foo() can be called for both rvalues and lvalues. But of cause we cannot differentiate behavior for lvalues and rvalues. We can only treat them the same way.

    If we implement only
        void foo(X&&)
    without void foo(X&) or void foo(const X&), foo() can only be called for rvalue. Calling it for lvalue will NOT compile!

    If we implement both
        void foo(X&) or void foo(const X&) and void foo(X&&)
    foo() can be called for both lvalues and rvalues. And you can distinguish between dealing with lvalues and rvalues. The version for rvalues is allowed to and should provide move semantics. Thus, it can steal the internal state and resources of the passed argument.

    This means that if a class does not provide move semantics and has only usual copy constructor and copy assignment operator, these will be called for rvalue references. Thus, std::move() means to call move semantics, if provided, and copy semantics otherwise.
    */
    string s1 {"good"};
    foo(s1);
    foo(s1 + s1);
    foo(move(s1));

    string s2 = foo1();
    cout << s2 << endl;

    return 0;
}

