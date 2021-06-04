#include <vector>
#include <string>

namespace abstract {
    
struct A {
    virtual ~A() {}
    virtual std::string f()=0;
};

struct B: A {
    std::string f() { return "B::f"; }
};

std::string call(A* a) { return a->f(); }
  
}
