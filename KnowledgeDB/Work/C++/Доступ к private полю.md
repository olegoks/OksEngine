```C++
#include <iostream>
#include "3rd_party_lib.hpp"

namespace pwn_helpers{
template<class Tag, auto MemberPtr>

struct MemberInfoStorage{

    friend constexpr auto GetMemberByTag(Tag){

        return MemberPtr;

    }

};

  

template<class Derived>

struct TagBase{

    friend constexpr auto GetMemberByTag(Derived);

};
}

struct A_Value_Tag : pwn_helpers::TagBase<A_Value_Tag>{};
struct A_Greet_Tag : pwn_helpers::TagBase<A_Greet_Tag>{};

template struct pwn_helpers::MemberInfoStorage<A_Value_Tag, &A::value>;
template struct pwn_helpers::MemberInfoStorage<A_Greet_Tag, &A::Greet>;

int main() {

    A a(42);
    std::cout << a.*GetMemberByTag(A_Value_Tag{}) << '\n';
    std::cout << (a.*GetMemberByTag(A_Greet_Tag{}))("World") << '\n';
    return 0;

}
```
