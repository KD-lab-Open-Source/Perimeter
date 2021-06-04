
struct Color
{
    Color(int r_ = 0, int g_ = 0, int b_ = 0):
        r(r_), g(g_), b(b_)
    {}        
    Color( const Color &c):
        r(c.r), g(c.g), b(c.b)
    {}
    int r;
    int g;
    int b;
};

extern const Color black;
extern const Color red;
extern const Color green;
extern const Color blue;
extern Color in_use;
