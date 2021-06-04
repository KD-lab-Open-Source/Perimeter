/* Copyright (c) 2002-2004 CrystalClear Software, Inc.
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE-1.0 or http://www.boost.org/LICENSE-1.0)
 * Author: Jeff Garland, Bart Garst
 * $Date: 2004/02/15 16:43:50 $
 */



#ifndef BOOST_DATE_TIME_SOURCE
#define BOOST_DATE_TIME_SOURCE
#endif
#include "boost/date_time/gregorian/greg_month.hpp"
#include "boost/date_time/gregorian/greg_facet.hpp"
#include "boost/date_time/date_format_simple.hpp"
#include "boost/date_time/compiler_config.hpp"
#if defined(BOOST_DATE_TIME_NO_WSTRING_CONVERSIONS)
#include "boost/date_time/gregorian/formatters_limited.hpp"
#else
#include "boost/date_time/gregorian/formatters.hpp"
#endif
#include "boost/date_time/date_parsing.hpp"

namespace boost {
namespace gregorian {

  const char* const short_month_names[NumMonths]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", "NAM"};
  const char* const long_month_names[NumMonths]={"January","February","March","April","May","June","July","August","September","October","November","December","NotAMonth"};

  const wchar_t* const w_short_month_names[NumMonths]={L"Jan",L"Feb",L"Mar",L"Apr",L"May",L"Jun",L"Jul",L"Aug",L"Sep",L"Oct",L"Nov",L"Dec",L"NAM"};
  const wchar_t* const w_long_month_names[NumMonths]={L"January",L"February",L"March",L"April",L"May",L"June",L"July",L"August",L"September",L"October",L"November",L"December",L"NotAMonth"};

  /*! Returns a shared pointer to a map of Month strings & numbers.
   * Strings are both full names and abbreviations.
   * Ex. ("jan",1), ("february",2), etc...
   * Note: All characters are lowercase - for case insensitivity
   */
  greg_month::month_map_ptr_type greg_month::get_month_map_ptr()
  {
    static month_map_ptr_type month_map_ptr(new greg_month::month_map_type())
;

    if(month_map_ptr->empty()) {
      std::string s("");
      for(unsigned short i = 1; i <= 12; ++i) {
        greg_month m(static_cast<month_enum>(i));
        s = m.as_long_string();
        s = date_time::convert_to_lower(s);
        month_map_ptr->insert(std::make_pair(s, i));
        s = m.as_short_string();
        s = date_time::convert_to_lower(s);
        month_map_ptr->insert(std::make_pair(s, i));
      }
    }
    return month_map_ptr;
  }
 


  //! Returns 3 char english string for the month ex: Jan, Feb, Mar, Apr
  const char*
  greg_month::as_short_string() const 
  {
    return short_month_names[value_-1];
  }
  
  //! Returns full name of month as string in english ex: January, February
  const char*
  greg_month::as_long_string()  const 
  {
    return long_month_names[value_-1];
  }


  //! Returns 3 wchar_t english string for the month ex: Jan, Feb, Mar, Apr
  const wchar_t*
  greg_month::as_short_wstring() const 
  {
    return w_short_month_names[value_-1];
  }
  
  //! Returns full name of month as wchar_t string in english ex: January, February
  const wchar_t*
  greg_month::as_long_wstring()  const 
  {
    return w_long_month_names[value_-1];
  }

} } //namespace gregorian






