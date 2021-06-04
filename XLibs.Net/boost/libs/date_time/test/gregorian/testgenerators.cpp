/* Copyright (c) 2002,2003 CrystalClear Software, Inc.
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE-1.0 or http://www.boost.org/LICENSE-1.0)
 * Author: Jeff Garland, Bart Garst 
 */

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/testfrmwk.hpp"
#include <iostream>
#include <string>
#include <sstream>

int
main() 
{

  using namespace boost::gregorian;

  partial_date pd1(1,Jan);
  date d = pd1.get_date(2000);
  check("Partial date getdate",     date(2000,1,1) == d);
  d = pd1.get_date(2001);
  check("Partial date getdate",     date(2001,1,1) == d);
  partial_date pd2(1,Feb);
  check("Partial date operator==",  pd1 == pd1);
  check("Partial date operator==",  !(pd1 == pd2));
  check("Partial date operator==",  !(pd2 == pd1));
  check("Partial date operator<",   !(pd1 < pd1));
  check("Partial date operator<",    pd1 < pd2);
  check("Partial date operator<",   !(pd2 < pd1));

  typedef last_day_of_the_week_in_month lastkday;

  //Find last Sunday in Feb
  lastkday lsif(Sunday, Feb);
  std::cout << to_simple_string(lsif.get_date(2002)) << std::endl; //24th
  check("Last kday",     date(2002,Feb,24) == lsif.get_date(2002));
  lastkday ltif(Thursday, Feb);
  check("Last kday",     date(2002,Feb,28) == ltif.get_date(2002));
  lastkday lfif(Friday, Feb);
  check("Last kday",     date(2002,Feb,22) == lfif.get_date(2002));

  typedef first_day_of_the_week_in_month firstkday;

  firstkday fsif(Sunday, Feb);
  std::cout << to_simple_string(fsif.get_date(2002)) << std::endl; //24th
  check("First kday",     date(2002,Feb,3) == fsif.get_date(2002));
  firstkday ftif(Thursday, Feb);
  check("First kday",     date(2002,Feb,7) == ftif.get_date(2002));
  firstkday ffif(Friday, Feb);
  check("First kday",     date(2002,Feb,1) == ffif.get_date(2002));
  
  typedef first_day_of_the_week_after firstkdayafter;
  firstkdayafter fkaf(Monday);
  std::cout << to_simple_string(fkaf.get_date(date(2002,Feb,1)))
            << std::endl; //feb 4
  check("kday after",date(2002,Feb,4) == fkaf.get_date(date(2002,Feb,1)));
  firstkdayafter fkaf2(Thursday);
  check("kday after",date(2002,Feb,7) == fkaf2.get_date(date(2002,Feb,1)));
  check("kday after",date(2002,Feb,28)== fkaf2.get_date(date(2002,Feb,21)));

  typedef first_day_of_the_week_before firstkdaybefore;
  firstkdaybefore fkbf(Monday);
  std::cout << to_simple_string(fkaf.get_date(date(2002,Feb,10)))
            << std::endl; //feb 4
  check("kday before",date(2002,Feb,4) == fkbf.get_date(date(2002,Feb,10)));
  firstkdaybefore fkbf2(Thursday);
  check("kday before",date(2002,Jan,31) == fkbf2.get_date(date(2002,Feb,1)));
  check("kday before",date(2002,Feb,7)== fkbf2.get_date(date(2002,Feb,14)));

  typedef nth_day_of_the_week_in_month nthkdayofmonth;
  nthkdayofmonth nkd1(nthkdayofmonth::third, Sunday, Jul);
  check("nth_kday 1", date(1969, Jul, 20) == nkd1.get_date(1969));
  nthkdayofmonth nkd2(nthkdayofmonth::second, Monday, Dec);
  check("nth_kday 2", date(1980, Dec, 8) == nkd2.get_date(1980));
  nthkdayofmonth nkd3(nthkdayofmonth::fifth, Wednesday, Jan);
  check("nth_kday fifth wed jan 2003 2003-Jan-29", 
        date(2003, Jan, 29) == nkd3.get_date(2003));
  nthkdayofmonth nkd4(nthkdayofmonth::fifth, Monday, Jan);
  check("nth_kday fifth mon jan 2003 (actaully 4th) 2003-Jan-27", 
        date(2003, Jan, 27) == nkd4.get_date(2003));

#ifndef BOOST_DATE_TIME_NO_LOCALE

  // streaming tests...
  std::stringstream ss("");
  std::string s("");
  
  ss.str("");
  ss << pd1;
  s = "1 Jan";
  check("streaming partial_date", ss.str() == s);
  
  ss.str("");
  ss << lsif;
  s = "last Sun of Feb";
  check("streaming last_kday_of_month", ss.str() == s);
  
  ss.str("");
  ss << fsif;
  s = "first Sun of Feb";
  check("streaming first_kday_of_month", ss.str() == s);
  
  ss.str("");
  ss << fkaf;
  s = "Mon after";
  check("streaming first_kday_after", ss.str() == s);
  
  ss.str("");
  ss << fkbf;
  s = "Mon before";
  check("streaming first_kday_before", ss.str() == s);
  
  ss.str("");
  ss << nkd1;
  s = "third Sun of Jul";
  check("streaming nth_kday", ss.str() == s);
#endif // NO_LOCAL
  
  return printTestStats();

}

