#include <boost/iterator/reverse_iterator.hpp>
#include <boost/cstdlib.hpp>

int main()
{
    typedef boost::reverse_iterator<int*>  rev_iter1;
    typedef boost::reverse_iterator<char*> rev_iter2;

    return boost::is_convertible<rev_iter1, rev_iter2>::value
        ? boost::exit_failure : boost::exit_success;
}
