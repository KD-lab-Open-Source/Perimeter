#ifndef NESTED_H
#define NESTED_H

namespace nested {
    
struct X
{
    struct Y 
    {
        int valueY;
        static int staticYValue; 
        struct Z
        {
            int valueZ;
        };
    };
    
    static int staticXValue;
    int valueX;
};

typedef X Root;

}

#endif
