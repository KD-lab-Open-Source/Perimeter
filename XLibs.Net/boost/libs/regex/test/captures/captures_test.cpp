
#include <boost/regex.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/array.hpp>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

template <class T>
void test_captures(const std::string& regx, const std::string& text, T& expected)
{
   boost::regex e(regx);
   boost::smatch what;
   if(boost::regex_match(text, what, e, boost::match_extra))
   {
      unsigned i, j;
      BOOST_TEST(what.size() == ARRAY_SIZE(expected));
      for(i = 0; i < what.size(); ++i)
      {
         BOOST_TEST(what.captures(i).size() <= ARRAY_SIZE(expected[i]));
         for(j = 0; j < what.captures(i).size(); ++j)
         {
            BOOST_TEST(what.captures(i)[j] == expected[i][j]);
         }
      }
   }
}

int test_main(int , char* [])
{
   typedef const char* pchar;
   pchar e1[4][5] = 
   {
      { "aBBcccDDDDDeeeeeeee", },
      { "a", "BB", "ccc", "DDDDD", "eeeeeeee", },
      { "a", "ccc", "eeeeeeee", },
      { "BB", "DDDDD", },
   };
   test_captures("(([[:lower:]]+)|([[:upper:]]+))+", "aBBcccDDDDDeeeeeeee", e1);
   pchar e2[4][2] = 
   {
      { "abd" },
      { "b", "" },
      { "" },
   };
   test_captures("a(b+|((c)*))+d", "abd", e2);
   pchar e3[3][1] = 
   {
      { "abcbar" },
      { "abc" },
   };
   test_captures("(.*)bar|(.*)bah", "abcbar", e3);
   pchar e4[3][1] = 
   {
      { "abcbah" },
      { 0, },
      { "abc" },
   };
   test_captures("(.*)bar|(.*)bah", "abcbah", e4);
   pchar e5[2][16] = 
   {
      { "now is the time for all good men to come to the aid of the party" },
      { "now", "is", "the", "time", "for", "all", "good", "men", "to", "come", "to", "the", "aid", "of", "the", "party" },
   };
   test_captures("^(?:(\\w+)|(?>\\W+))*$", "now is the time for all good men to come to the aid of the party", e5);
   pchar e6[2][16] = 
   {
      { "now is the time for all good men to come to the aid of the party" },
      { "now", "is", "the", "time", "for", "all", "good", "men", "to", "come", "to", "the", "aid", "of", "the", "party" },
   };
   test_captures("^(?>(\\w+)\\W*)*$", "now is the time for all good men to come to the aid of the party", e6);
   pchar e7[4][14] = 
   {
      { "now is the time for all good men to come to the aid of the party" },
      { "now" },
      { "is", "the", "time", "for", "all", "good", "men", "to", "come", "to", "the", "aid", "of", "the" },
      { "party" },
   };
   test_captures("^(\\w+)\\W+(?>(\\w+)\\W+)*(\\w+)$", "now is the time for all good men to come to the aid of the party", e7);
   pchar e8[5][9] = 
   {
      { "now is the time for all good men to come to the aid of the party" } ,
      { "now" },
      { "is", "for", "men", "to", "of" },
      { "the", "time", "all", "good", "to", "come", "the", "aid", "the" },
      { "party" },
   };
   test_captures("^(\\w+)\\W+(?>(\\w+)\\W+(?:(\\w+)\\W+){0,2})*(\\w+)$", "now is the time for all good men to come to the aid of the party", e8);
   return 0;
}

