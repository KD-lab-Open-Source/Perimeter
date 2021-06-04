/* (C) Copyright Rene Rivera, 2002.
**
** See accompanying license for terms and conditions of use.
*/

#include <cstdio>

extern int lib_call(int x);

int main(int /* argc */, char ** /* argv */)
{
    for (int i = 0; i < 16; ++i)
    {
        std::printf("%d * 2 = %d\n",i,lib_call(i));
    }
    return 0;
}
