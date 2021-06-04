
namespace inherit3 {
    
struct A
{
    struct X { int y; };
    int x;
    virtual int foo() { return 0; }
    virtual int foo(int x) { return x; }
    A operator+(A o) const
    {
        A r;
        r.x = o.x + x;
        return r;
    }
    enum E { i, j };

};

struct B: A
{
    struct X { int y; };
    int x;
    int foo() { return 1; }
    A operator+(A o) const
    {
        A r;
        r.x = o.x + x;
        return r;
    }     
    enum E { i, j };

}; 

struct C: A
{
};  

}
