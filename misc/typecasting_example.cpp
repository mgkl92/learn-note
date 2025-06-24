#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

/**
 * 1. Type Casting
 *
 * Reference(s):
 * https://cplusplus.com/doc/tutorial/typecasting/
 */

// class Dummy {
//   double i, j;
// };

// class Addition {
//   int x, y;

//  public:
//   Addition(int a, int b) {
//     x = a;
//     y = b;
//   }

//   int result() {
//     return x + y;
//   }
// };

// int main(int argc, char const *argv[]) {
//   Dummy d;
//   Addition *padd;
//   padd = (Addition *)&d;

//   // run-time error or unexcepted results
//   // Output: 8
//   cout << padd->result();

//   return 0;
// }

// class Base {
//   virtual void dummy() {}
// };

// class Derived : public Base {
//   int a;
// };

// int main(int argc, char const *argv[]) {
//   try {
//     Base *pba = new Derived;
//     Base *pbb = new Base;
//     Derived *pd;

//     pd = dynamic_cast<Derived *>(pba);
//     if (pd == 0) {
//       cout << "Null pointer on first type-cast.\n";
//     }

//     pd = dynamic_cast<Derived *>(pbb);
//     if (pd == 0) {
//       cout << "Null pointer on second type-cast.\n";
//     }
//   } catch (exception &e) {
//     cout << "Exception: " << e.what() << '\n';
//   }

//   return 0;
// }

/**
 * 2. dynamic_cast<target_type>(expression)
 *
 * Description: Safely converts pointers and references to classes up/down and
 * sideways along the inheritance hierarchy.
 *
 * Note: dynamic_cast requires Run-Time Type Information(RTTI) to keep track of
 * the dynamic types.
 *
 * Reference(s):
 * https://www.en.cppreference.com/w/cpp/language/dynamic_cast.html
 */

// struct V {
//   virtual void f() {}
// };

// struct A : virtual V {};

// struct B : virtual V {
//   B(V *v, A *a) {
//     dynamic_cast<B *>(v); // downcast: V base of B, v of type V*, results in
//     B* dynamic_cast<B *>(a); // sidecast: undefined behavior
//   }
// };

// struct D : A, B {
//   D() : B(static_cast<A *>(this), this) {}
// };

// struct Base {
//   virtual ~Base() {};
// };

// struct Derived : Base {
//   virtual void name() {};
// };

// /**
//  * Output:
//  *
//  * downcast from b2 to d successful
//  */
// int main(int argc, char const *argv[]) {
//   D d;
//   A &a = d; // upcast

//   D &new_d = dynamic_cast<D &>(a); // downcast
//   B &new_b = dynamic_cast<B &>(a); // sidecast

//   Base *b1 = new Base;
//   if (Derived *d = dynamic_cast<Derived *>(b1); d != nullptr) {
//     std::cout << "downcast from b1 to d successful \n";
//     d->name();
//   }

//   // dynamic_cast can downcast if-and-only-if the pointed object is a valid
//   complete object of that target type. Base *b2 = new Derived; if (Derived *d
//   = dynamic_cast<Derived *>(b2); d != nullptr) {
//     std::cout << "downcast from b2 to d successful \n";
//     d->name();
//   }

//   delete b1;
//   delete b2;

//   return 0;
// }

/**
 * 3. static_cast<target_type>(expression)
 *
 * Description: Converts between types using a combination of implicit and
 * user-defined conversions.
 *
 * Reference(s):
 * https://www.en.cppreference.com/w/cpp/language/static_cast.html
 */

// struct Base {
//   int m = 42;
//   const char *hello() const { return "Hello world, this is Base!\n"; }
// };

// struct Derived : Base {
//   const char *hello() const { return "Hello world, this is Derived!\n"; }
// };

// enum class E { ONE = 1, TWO, THREE };
// enum EU { ONE, TWO, THREE };

// /**
//  * Output
//  *
//  * 1) Hello world, this is Base!
//  * 0x33c49ffa04
//  * 2) Hello world, this is Derived!
//  * 0x33c49ffa04
//  * 3) after move, v0.size() = 0
//  * 5) n = 3
//  * 5) v.size() = 10
//  * 6) *ni = 3
//  * 7a) 2
//  * 7f) 42
//  */
// int main(int argc, char const *argv[]) {
//   //  1. static downcast
//   Derived d;
//   Base &br = d; // upcast(implicit conversion)
//   std::cout << "1) " << br.hello();
//   std::cout << &br << '\n';

//   Derived &another_d = static_cast<Derived &>(br); // downcast
//   std::cout << "2) " << another_d.hello();
//   std::cout << &another_d << '\n';

//   // 3. lvalue to xvalue
//   std::vector<int> v0{1, 2, 3};
//   std::vector<int> v2 = static_cast<std::vector<int> &&>(v0);
//   std::cout << "3) after move, v0.size() = " << v0.size() << '\n';

//   // 4. discarded-value expression
//   // call to some function, but ignore its returned value
//   // to elimite the warning from compiler
//   static_cast<void>(v2.size());

//   // 5. initializing conversion
//   int n = static_cast<int>(3.14);
//   std::cout << "5) n = " << n << '\n';
//   std::vector<int> v = static_cast<std::vector<int>>(10);
//   std::cout << "5) v.size() = " << v.size() << '\n';

//   // 6. inverse of implicit conversion
//   void *nv = &n;
//   int *ni = static_cast<int *>(nv);
//   std::cout << "6) *ni = " << *ni << '\n';

//   // 7a. scoped enum to int
//   E e = E::TWO;
//   int two = static_cast<int>(e);
//   std::cout << "7a) " << two << '\n';

//   // 7b. in to enum, enum to another enum
//   E e2 = static_cast<E>(two);
//   EU eu = static_cast<EU>(e2);

//   // 7f. pointer to member upcast
//   int Derived::*pm = &Derived::m;
//   std::cout << "7f) " << br.*static_cast<int Base::*>(pm) << '\n';

//   // 7g. void* to any object pointer
//   void *voidp = &e;
//   std::vector<int> *p = static_cast<std::vector<int> *>(voidp);
//   return 0;
// }

/**
 * 4. const_cast<target_type>(expression)
 *
 * Description: Converts between types with different cv-qualification.
 *
 * Note:
 * 1) `const_cast` can only adjust type qualifiers, it cannot change the
 * underlying type.
 * 2) Removing the constness of a pointed object to actually write to it cause
 * undefined behavior.
 *
 * Reference(s):
 * https://www.en.cppreference.com/w/cpp/language/const_cast.html
 */

// struct type {
//   int i;

//   type() : i(3) {}

//   void f(int v) const {
//     // this->i = v; // compiler error: const type *this
//     const_cast<type *>(this)->i = v;
//   }
// };

/**
 * Output
 *
 * i = 4
 * type::i(before) = 3
 * type::i(after) = 4
 */
// int main(int argc, char const *argv[]) {
//   int i = 3;
//   const int &rci = i;
//   const_cast<int &>(rci) = 4; // OK
//   std::cout << "i = " << i << '\n';

//   type t;
//   std::cout << "type::i(before) = " << t.i << '\n';
//   t.f(4);
//   std::cout << "type::i(after) = " << t.i << '\n';

//   const int j = 3;
//   int *pj = const_cast<int *>(&j);
//   *pj = 4; // Undefined behavior

//   void (type::*pmf)(int) const = &type::f;
//   //   const_cast<void(type::*)(int)>(pmf); // error: const_cast does not
//   work
//   //   on function pointers
//   // the `const` is guaranteed by a const `this` pointer, not the function
//   // itself
//   return 0;
// }

/**
 * 5. reinterpret_cast<target_type>(expression)
 *
 * Description: Converts between types by reinterpreting the underlying bit
 * pattern.
 *
 * Reference(s):
 * https://www.en.cppreference.com/w/cpp/language/reinterpret_cast.html
 */

// int f() { return 42; }

// /**
//  * Output
//  *
//  * The value of &i is 0x58be1ff7dc
//  * fp2() = 42
//  * This system is little-endian.
//  * i = 42
//  */
// int main(int argc, char const *argv[]) {
//   int i = 7;

//   // 1. pointer to integer and back
//   // typedef unsigned long long uintptr_t
//   std::uintptr_t v1 = reinterpret_cast<std::uintptr_t>(&i);
//   std::cout << "The value of &i is " << std::showbase << std::hex << v1 <<
//   '\n'; int *p1 = reinterpret_cast<int *>(v1); assert(p1 == &i);

//   // 2. pointer to function to another and back
//   void (*fp1)() = reinterpret_cast<void (*)()>(f);
//   // fp1(); // undefined behavior
//   int (*fp2)() = reinterpret_cast<int (*)()>(fp1);
//   std::cout << "fp2() = " << std::dec << fp2() << '\n'; // OK

//   // 3. type aliasing through pointer
//   char *p2 = reinterpret_cast<char *>(&i);
//   std::cout << (p2[0] == '\x7' ? "This system is little-endian.\n"
//                                : "This system is big-endian.\n");

//   // 4. type aliasing through reference
//   reinterpret_cast<unsigned int &>(i) = 42;
//   std::cout << "i = " << i << '\n';

//   const int &const_iref = i;
//   //  int &iref = reinterpret_cast<int &>(const_iref);
//   // error: reinterpret_cast cannot cast away const or other type qualifiers

//   return 0;
// }

/**
 * 6. typeid(expression)
 *
 * Description: typeid allows to check the type of an expression.
 *
 * Note:If the type typeid evaluates is a pointer preceded by the dereference
 * operator (*), and this pointer has a null value, typeid throws a bad_typeid
 * exception.
 */

// class Base {
//   virtual void f() {}
// };

// class Derived : public Base {};

// /**
//  * Output
//  *
//  * a and b are different types:
//  * a is: Pi
//  * b is: i
//  * a is: P4Base
//  * b is: P4Base
//  * *a is: 4Base
//  * *b is: 7Derived
//  * *c is: Exception: std::bad_typeid
//  */
// int main(int argc, char const *argv[]) {
//   int *a, b;

//   a = 0;
//   b = 0;

//   if (typeid(a) != typeid(b)) {
//     cout << "a and b are different types: \n";
//     cout << "a is: " << typeid(a).name() << '\n';
//     cout << "b is: " << typeid(b).name() << '\n';
//   }

//   try {
//     Base *a = new Base;
//     Base *b = new Derived;
//     Base *c = nullptr;

//     cout << "a is: " << typeid(a).name() << '\n';
//     cout << "b is: " << typeid(b).name() << '\n';
//     cout << "*a is: " << typeid(*a).name() << '\n';
//     cout << "*b is: " << typeid(*b).name() << '\n';

//     // *c is: Exception: std::bad_typeid
//     cout << "*c is: " << typeid(*c).name() << '\n';
//   } catch (exception &e) {
//     cout << "Exception: " << e.what() << '\n';
//   }

//   return 0;
// }