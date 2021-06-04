/*
 *
 * Copyright (c) 1998-2002
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         posix_api_compiler_check.c
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Verify that POSIX API calls compile: note this is a compile
  *                time check only.
  */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <boost/regex.h>

const char* expression = "^";
const char* text = "\n      ";
regmatch_t matches[1];
int flags = REG_EXTENDED | REG_BASIC | REG_NOSPEC | REG_ICASE | REG_NOSUB |
            REG_NEWLINE | REG_PEND | REG_NOCOLLATE | REG_ESCAPE_IN_LISTS |
            REG_NEWLINE_ALT | REG_PERL | REG_AWK | REG_GREP | REG_EGREP;


int main()
{
   regex_t re;
   int result;
   result = regcomp(&re, expression, REG_AWK);
   if(result > REG_NOERROR)
   {
      char buf[256];
      regerror(result, &re, buf, sizeof(buf));
      printf(buf);
      return result;
   }
   assert(re.re_nsub == 0);
   matches[0].rm_so = 0;
   matches[0].rm_eo = strlen(text);
   result = regexec(&re, text, 1, matches, REG_NOTBOL | REG_NOTEOL | REG_STARTEND);
   if(result > REG_NOERROR)
   {
      char buf[256];
      regerror(result, &re, buf, sizeof(buf));
      printf(buf);
      regfree(&re);
      return result;
   }
   assert(matches[0].rm_so == matches[0].rm_eo == 1);
   regfree(&re);
   printf("no errors found\n");
   return 0;
}



