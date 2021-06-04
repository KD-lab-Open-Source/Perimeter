/*
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */ 
// WARNING: This is an internal header file, included by other C++
// standard library headers.  You should not attempt to use this header
// file directly.


#ifndef _STLP_INTERNAL_LOCALE_H
#define _STLP_INTERNAL_LOCALE_H

#ifndef _STLP_CSTDLIB
#  include <cstdlib>
#endif

#ifndef _STLP_CWCHAR_H
#  include <stl/_cwchar.h>
#endif

#ifndef _STLP_INTERNAL_THREADS_H
#  include <stl/_threads.h>
#endif

#ifndef _STLP_STRING_FWD_H
#  include <stl/_string_fwd.h>
#endif

_STLP_BEGIN_NAMESPACE

class _STLP_CLASS_DECLSPEC _Locale_impl;        // Forward declaration of opaque type.
class _STLP_CLASS_DECLSPEC ios_base;

#if defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
class locale;
#  define locale _STLP_NO_MEM_T_NAME(loc)
class _STLP_CLASS_DECLSPEC locale;
#else
class _STLP_CLASS_DECLSPEC locale;
#endif /* _STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND */

template <class _CharT, class _Traits, class _Alloc>
bool 
__locale_do_operator_call (const locale* __that, 
                           const basic_string<_CharT, _Traits, _Alloc>& __x,
                           const basic_string<_CharT, _Traits, _Alloc>& __y);

_STLP_DECLSPEC _Locale_impl * _STLP_CALL _get_Locale_impl( _Locale_impl *locimpl );
_STLP_DECLSPEC _Locale_impl * _STLP_CALL _copy_Nameless_Locale_impl( _Locale_impl *locimpl );

class _STLP_CLASS_DECLSPEC locale {
public:
  // types:
  class _STLP_DECLSPEC facet : protected _Refcount_Base {
  protected:
    explicit facet(size_t __no_del = 0) : _Refcount_Base( __no_del == 0 ? 0 : 1 ), _M_delete(__no_del == 0) {}
    virtual ~facet();
    friend class locale;
    friend class _Locale_impl;
    friend facet * _STLP_CALL _get_facet( facet * );
    friend void _STLP_CALL _release_facet( facet *& );
    
  private:                        // Invalidate assignment and copying.
    facet(const facet& ) /* : _Refcount_Base(1), _M_delete(false) {} */;
    void operator=(const facet&); 
    
  private:                        // Data members.
    const bool _M_delete;
  };
  
#if defined(__MVS__) || defined(__OS400__)
  struct
#else
  class
#endif
  _STLP_DECLSPEC id {
    friend class locale;
    friend class _Locale_impl;
  public:
    size_t _M_index;
    static size_t _S_max;
  };

  typedef int category;
#if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _Category {
#else
  static const category
#endif
    none      = 0x000,
    collate   = 0x010,
    ctype     = 0x020,
    monetary  = 0x040,
    numeric   = 0x100,
    time      = 0x200,
    messages  = 0x400,
    all       = collate | ctype | monetary | numeric | time | messages
#if defined (_STLP_STATIC_CONST_INIT_BUG)
  }
#endif
  ;

  // construct/copy/destroy:
  locale();
  locale(const locale&) _STLP_NOTHROW;
  explicit locale(const char *);
  locale(const locale&, const char*, category);

#if defined (_STLP_MEMBER_TEMPLATES) && !defined(_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  template <class _Facet>
  locale(const locale& __loc, _Facet* __f) {
    if ( __f != 0 ) {
      this->_M_impl = _get_Locale_impl( _copy_Nameless_Locale_impl( __loc._M_impl ) );
      this->_M_insert(__f, _Facet::id);
    } else {
      this->_M_impl = _get_Locale_impl( __loc._M_impl );
    }
  }
#endif // _STLP_MEMBER_TEMPLATES

protected:
  // those are for internal use
  locale(_Locale_impl*);

public:

  locale(const locale&, const locale&, category);
  const locale& operator=(const locale&) _STLP_NOTHROW;

#if defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  virtual
#endif
   ~locale() _STLP_NOTHROW;

#if defined (_STLP_MEMBER_TEMPLATES) && !defined (_STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS) && \
   !defined(_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
  template <class _Facet>
  locale combine(const locale& __loc) {
    facet* __f = __loc._M_get_facet( _Facet::id );
    if ( __f == 0 )
      _M_throw_runtime_error();

    locale __result(__loc._M_impl);
    
    __result._M_insert(__f, _Facet::id);

    return __result;
  }
#endif // _STLP_MEMBER_TEMPLATES && !_STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS

  // locale operations:
  string name() const;

  bool operator==(const locale&) const;
  bool operator!=(const locale&) const;

#if ! defined ( _STLP_MEMBER_TEMPLATES ) || defined (_STLP_INLINE_MEMBER_TEMPLATES) || (defined(__MWERKS__) && __MWERKS__ <= 0x2301)
  bool operator()(const string& __x, const string& __y) const;
#  ifndef _STLP_NO_WCHAR_T
  bool operator()(const wstring& __x, const wstring& __y) const;
#  endif
#else
  template <class _CharT, class _Traits, class _Alloc>
  bool operator()(const basic_string<_CharT, _Traits, _Alloc>& __x,
                  const basic_string<_CharT, _Traits, _Alloc>& __y) const {
    return __locale_do_operator_call(this, __x, __y);
  }              
#endif

  // global locale objects:
  static locale _STLP_CALL global(const locale&);
  static const locale& _STLP_CALL classic();

//protected:                         // Helper functions for locale globals.
  facet* _M_get_facet(const id&) const;
  // same, but throws
  facet* _M_use_facet(const id&) const;
  static void _STLP_CALL _M_throw_runtime_error(const char* = 0);

protected:                        // More helper functions.
  void _M_insert(facet* __f, id& __id);

  // friends:
  friend class _Locale_impl;
  friend class ios_base;

protected:                        // Data members
  _Locale_impl* _M_impl;
  _Locale_impl* _M_get_impl() const { return _M_impl; }
};

#if defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
#  undef locale
#  define _Locale _STLP_NO_MEM_T_NAME(loc)

class locale : public _Locale {
public:

  // construct/copy/destroy:
  locale() {}
  locale(const locale& __loc) _STLP_NOTHROW : _Locale(__loc) {}
  explicit locale(const char *__str) : _Locale(__str) {}
  locale(const locale& __loc, const char* __str, category __cat)
    : _Locale(__loc, __str, __cat) {}

  template <class _Facet>
  locale(const locale& __loc, _Facet* __f) {
    if ( __f != 0 ) {
      this->_M_impl = _get_Locale_impl( _copy_Nameless_Locale_impl( __loc._M_impl ) );
      this->_M_insert(__f, _Facet::id);
    } else {
      this->_M_impl = _get_Locale_impl( __loc._M_impl );
    }
  }

private:
  // those are for internal use
  locale(_Locale_impl* __impl) : _Locale(__impl) {}
  locale(const _Locale& __loc) : _Locale(__loc) {}

public:

  locale(const locale& __loc1, const locale& __loc2, category __cat)
    : _Locale(__loc1, __loc2, __cat) {}

  const locale& operator=(const locale& __loc) _STLP_NOTHROW {
    _Locale::operator=(__loc);
    return *this;
  }

  template <class _Facet>
  locale combine(const locale& __loc) {
    facet* __f = __loc._M_get_facet( _Facet::id );
    if ( __f == 0 )
      _M_throw_runtime_error();

    locale __result(__loc._M_impl);
    
    __result._M_insert(__f, _Facet::id);

    return __result;
  }

  // locale operations:
  bool operator==(const locale& __loc) const { return _Locale::operator==(__loc); }
  bool operator!=(const locale& __loc) const { return _Locale::operator!=(__loc); }

  // global locale objects:
  static locale _STLP_CALL global(const locale& __loc) {
    return _Locale::global(__loc);
  }
  static const locale& _STLP_CALL classic() {
    return __STATIC_CAST(const locale&, _Locale::classic());
  }

  // friends:
  friend class _Locale_impl;
  friend class ios_base;
};

#endif /* _STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND */


//----------------------------------------------------------------------
// locale globals

# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
template <class _Facet>
inline const _Facet& 
_Use_facet<_Facet>::operator *() const
# else
template <class _Facet> inline const _Facet& use_facet(const locale& __loc)
# endif
{
  return *__STATIC_CAST(const _Facet*,__loc._M_use_facet(_Facet::id));
}

 
# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
template <class _Facet> 
struct has_facet {
  const locale& __loc;
  has_facet(const locale& __p_loc) : __loc(__p_loc) {}
  operator bool() const _STLP_NOTHROW
# else
template <class _Facet> inline bool has_facet(const locale& __loc) _STLP_NOTHROW 
# endif
{
  return (__loc._M_get_facet(_Facet::id) != 0);
}

# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
  // close class definition
};
# endif

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_LOCALE_H */

// Local Variables:
// mode:C++
// End:

