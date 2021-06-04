namespace inherit2 {

struct A
{
    int x;
    int getx() { return x; }
    int foo() { return 0; }
    int foo(int x) { return x; }
};

struct B : A
{
    int y;
    int gety() { return y; }
    int foo() { return 1; }
};

struct C : B
{
    int z;
    int getz() { return z; }
};

struct D : C
{
    int w;
    int getw() { return w; }
};

}
