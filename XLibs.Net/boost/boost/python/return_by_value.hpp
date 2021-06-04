// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#ifndef BY_VALUE_DWA20021015_HPP
# define BY_VALUE_DWA20021015_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/to_python_value.hpp>
# include <boost/type_traits/add_reference.hpp>
# include <boost/type_traits/add_const.hpp>

# include <boost/python/detail/value_arg.hpp>

namespace boost { namespace python { 

struct return_by_value
{
    template <class R>
    struct apply
    {
       typedef to_python_value<
           typename detail::value_arg<R>::type
       > type;
    };
};

}} // namespace boost::python

#endif // BY_VALUE_DWA20021015_HPP
