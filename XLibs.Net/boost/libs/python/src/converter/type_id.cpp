// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/type_id.hpp>
#include <boost/python/detail/decorated_type_id.hpp>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <cstring>

#if !defined(__GNUC__) || __GNUC__ >= 3 || __SGI_STL_PORT || __EDG_VERSION__
# include <ostream>
#else 
# include <ostream.h>
#endif 


#  ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
#   if defined(__GNUC__) &&  __GNUC__ >= 3
#    include <cxxabi.h>
#   endif
#  endif 

namespace boost { namespace python {

#  ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
#   ifdef __GNUC__
#    if __GNUC__ < 3

namespace cxxabi = :: ;
extern "C" char* __cxa_demangle(char const*, char*, std::size_t*, int*);
#    else

namespace cxxabi = ::abi;       // GCC 3.1 and later

#     if __GNUC__ == 3 && __GNUC_MINOR__ == 0
namespace abi
{
  extern "C" char* __cxa_demangle(char const*, char*, std::size_t*, int*);
}
#     endif 
#    endif
#   endif 

namespace
{
  struct compare_first_cstring
  {
      template <class T>
      bool operator()(T const& x, T const& y)
      {
          return std::strcmp(x.first,y.first) < 0;
      }
  };
  
  struct free_mem
  {
      free_mem(char*p)
          : p(p) {}
    
      ~free_mem()
      {
          std::free(p);
      }
      char* p;
  };
}

namespace detail
{
  BOOST_PYTHON_DECL char const* gcc_demangle(char const* mangled)
  {
      typedef std::vector<
          std::pair<char const*, char const*>
      > mangling_map;
      
      static mangling_map demangler;
      mangling_map::iterator p
          = std::lower_bound(
              demangler.begin(), demangler.end()
            , std::make_pair(mangled, (char const*)0)
            , compare_first_cstring());
      
      if (p == demangler.end() || strcmp(p->first, mangled))
      {
          int status;
          free_mem keeper(
              cxxabi::__cxa_demangle(mangled, 0, 0, &status)
              );
    
          assert(status != -3); // invalid argument error
    
          if (status == -1)
          {
              throw std::bad_alloc();
          }
          else
          {
              char const* demangled
                = status == -2
                  // Invalid mangled name.  Best we can do is to
                  // return it intact.
                  ? mangled
                  : keeper.p;

              p = demangler.insert(p, std::make_pair(mangled, demangled));
              keeper.p = 0;
          }
      }
      
      return p->second;
  }
}
#  endif

BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream& os, type_info const& x)
{
    return os << x.name();
}

namespace detail
{
  BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream& os, detail::decorated_type_info const& x)
  {
      os << x.m_base_type;
      if (x.m_decoration & decorated_type_info::const_)
          os << " const";
      if (x.m_decoration & decorated_type_info::volatile_)
          os << " volatile";
      if (x.m_decoration & decorated_type_info::reference)
          os << "&";
      return os;
  }
}
}} // namespace boost::python::converter
