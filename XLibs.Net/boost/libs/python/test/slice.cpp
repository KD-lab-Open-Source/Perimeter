#include <boost/python.hpp>
#include <boost/python/slice.hpp>

using namespace boost::python;

// These checks are only valid under Python 2.3
// (rich slicing wasn't supported for builtins under Python 2.2)
bool check_string_rich_slice()
{
    object s("hello, world");

    // default slice
    if (s[slice()] != "hello, world")
        return false;

    // simple reverse
    if (s[slice(_,_,-1)] != "dlrow ,olleh")
        return false;

    // reverse with mixed-sign offsets
    if (s[slice(-6,1,-1)] != " ,oll")
        return false;

    // all of the object.cpp check_string_slice() checks should work
    // with the form that omits the step argument.
    if (s[slice(_,-3)] != "hello, wo")
        return false;
    if (s[slice(-3,_)] != "rld")
        return false;
    if (", " != s[slice(5,7)])
        return false;

    return s[slice(2,-1)][slice(1,-1)]  == "lo, wor";
}

// These tests work with Python 2.2, but you must have Numeric installed.
bool check_numeric_array_rich_slice()
{
    using numeric::array;
    array original = array( make_tuple( make_tuple( 11, 12, 13, 14),
                                        make_tuple( 21, 22, 23, 24),
                                        make_tuple( 31, 32, 33, 34),
                                        make_tuple( 41, 42, 43, 44)));
    array upper_left_quadrant = array( make_tuple( make_tuple( 11, 12),
                                                   make_tuple( 21, 22)));
    array odd_cells = array( make_tuple( make_tuple( 11, 13),
                                          make_tuple( 31, 33)));
    array even_cells = array( make_tuple( make_tuple( 22, 24),
                                           make_tuple( 42, 44)));
    array lower_right_quadrant_reversed = array(
        make_tuple( make_tuple(44, 43),
                    make_tuple(34, 33)));

    // The following comments represent equivalent Python expressions used
    // to validate the array behavior.
    // original[::] == original
    if (original[slice()] != original)
        return false;
    // original[:2,:2] == array( [[11, 12], [21, 22]])
    if (original[make_tuple(slice(_,2), slice(_,2))] != upper_left_quadrant)
        return false;
    // original[::2,::2] == array( [[11, 13], [31, 33]])
    if (original[make_tuple( slice(_,_,2), slice(_,_,2))] != odd_cells)
        return false;
    // original[1::2, 1::2] == array( [[22, 24], [42, 44]])
    if (original[make_tuple( slice(1,_,2), slice(1,_,2))] != even_cells)
        return false;
    // original[:-3:-1, :-3,-1] == array( [[44, 43], [34, 33]])
    if (original[make_tuple( slice(_,-3,-1), slice(_,-3,-1))] != lower_right_quadrant_reversed)
        return false;

    return true;
}

// Verify functions accepting a slice argument can be called
bool accept_slice( slice) { return true; }

BOOST_PYTHON_MODULE(slice_ext)
{
    def( "accept_slice", accept_slice);
    def( "check_numeric_array_rich_slice", check_numeric_array_rich_slice);
    def( "check_string_rich_slice", check_string_rich_slice);
}
