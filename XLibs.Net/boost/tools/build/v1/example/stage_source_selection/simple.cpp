/* (C) Copyright Rene Rivera, 2002.
**
** See accompanying license for terms and conditions of use.
*/

#include <cstdio>

int main(int /* argc */, char ** /* argv */)
{
    for (int i = 0; i < 16; ++i)
    {
        std::printf("%d * 2 = %d\n",i,i*2);
    }
    return 0;
}
