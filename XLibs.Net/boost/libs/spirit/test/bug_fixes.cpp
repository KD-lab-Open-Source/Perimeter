/*=============================================================================
    Copyright (c) 2003 Giovanni Bajo
    Copyright (c) 2003 Joel de Guzman
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

using namespace boost;
using namespace boost::spirit;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
//  bug_001
//
//  access_node_d[] and access_match_d[] iterator bug
//  http://sf.net/mailarchive/forum.php?thread_id=1963157&forum_id=1595
//  http://sf.net/mailarchive/forum.php?thread_id=1966224&forum_id=1595
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/tree/ast.hpp>

struct my_action
{
    template <typename TreeT, typename IterT>
    void operator()(TreeT& /*t*/, IterT begin, IterT end) const
    {
        BOOST_TEST(*begin == '1');
        BOOST_TEST(*end == '2');
    }
};

void bug_001()
{
    const char* text = "123";

    ast_parse(text, text+3, access_node_d[chlit<>('1')][my_action()]);
    ast_parse(text, text+3, access_match_d[chlit<>('1')][my_action()]);
}

///////////////////////////////////////////////////////////////////////////////
//
//  bug_001
//
//  mismatch closure return type bug
//  http://article.gmane.org/gmane.comp.parsers.spirit.general/3678
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/attribute.hpp>
#include <string>

typedef std::string member_type;

struct my_closure: closure<my_closure, member_type>
{
    member1 val;
};

void bug_002()
{
    rule<scanner<char const*>, my_closure::context_t> my_rule = real_p;
    BOOST_TEST(parse("1", my_rule).full);
}

///////////////////////////////////////////////////////////////////////////////
//
//  bug_003
//
//  impl::detach_clear bug
//  http://sourceforge.net/mailarchive/forum.php?thread_id=2008510&forum_id=25901
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/utility/chset.hpp>

void bug_003()
{
    chset<> set;
    set = 'a';
}

///////////////////////////////////////////////////////////////////////////////
//
//  bug_004
//
//  chset<>::operator~(range<>) bug
//  operator&(chset<>, range<>) bug
//  operator&(range<>, chset<>) bug
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/limits.hpp>
#include <boost/spirit/utility/chset.hpp>

void bug_004()
{
    const char min = (numeric_limits<char>::min)();
    const char max = (numeric_limits<char>::max)();

    {
        chset<> set(~range<>(min, max));
        BOOST_CHECK(set.test(min) == false);
        BOOST_CHECK(set.test(min) == false);
    }

    {
        chset<> set(chset<>(anychar_p) & range<>(min, max));
        BOOST_CHECK(set.test(min) == true);
        BOOST_CHECK(set.test(min) == true);
    }

    {
        chset<> set(range<>(min, max) & chset<>(anychar_p));
        BOOST_CHECK(set.test(min) == true);
        BOOST_CHECK(set.test(min) == true);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  bug_005
//
//  Most trailing space bug
//  http://article.gmane.org/gmane.comp.parsers.spirit.general/4029
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/core.hpp>

using namespace std;
using namespace boost;
using namespace unit_test_framework;
using namespace spirit;

void bug_005()
{
    BOOST_CHECK(
        parse("   aaaaaaaaa     ", *ch_p('a'), space_p).full
    );

    BOOST_CHECK(
        parse("   aaaaaaaaa     ", lexeme_d[*ch_p('a')], space_p).full
    );

    BOOST_CHECK(
        parse("   aaaaaaaaa     ", *ch_p('a'), ch_p(' ')).full
    );

    BOOST_CHECK(
        parse("   aaaaaaaaa     ", lexeme_d[*ch_p('a')], ch_p(' ')).full
    );
}

///////////////////////////////////////////////////////////////////////////////
//
//  bug_006
//
//  confix bug
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/limits.hpp>
#include <boost/spirit/utility/confix.hpp>

void bug_006()
{
    BOOST_CHECK(parse("#some comment", comment_p('#')).full);
}

///////////////////////////////////////////////////////////////////////////////
//
//  bug_007
//
//  handling of trailing whitespace bug (ast_parse/pt_parse related)
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/parse_tree.hpp>

void bug_007()
{
    BOOST_CHECK(parse("test ", str_p("test"), space_p).full);
    BOOST_CHECK(pt_parse("test ", str_p("test"), space_p).full);
    BOOST_CHECK(ast_parse("test ", str_p("test"), space_p).full);
}

///////////////////////////////////////////////////////////////////////////////
//
//  sf_bug_718903
//
//  see https://sourceforge.net/tracker/index.php
//  ?func=detail&aid=718903&group_id=28447&atid=393386
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/cstdlib.hpp>
#include <boost/spirit/utility/chset.hpp>

void sf_bug_718903()
{
    empty_match_parser<chset<char> >
        e(epsilon_p(chset_p("abc")));
}

///////////////////////////////////////////////////////////////////////////////
//
//  sf_bug_719322
//  range_run bug
//
//  see http://sourceforge.net/tracker/index.php
//  ?func=detail&aid=719322&group_id=28447&atid=393386
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/utility/impl/chset/basic_chset.hpp>

void sf_bug_719322()
{
    basic_chset<int> s;
    s.set(3, 3);
    s.set(1, 5);
    BOOST_CHECK(s.test(5));
}

///////////////////////////////////////////////////////////////////////////////
//
//  sf_bug_742038
//
//  see http://sf.net/tracker/
//  ?func=detail&atid=393386&aid=742038&group_id=28447
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/iterator/position_iterator.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <stdio.h>

template <typename IterT>
void test_assign(IterT b, IterT e)
{
    typedef scanner<IterT> scanner_t;

#if     (defined(__GNUC__) && defined(__MINGW32__)) \
    ||  (defined(__GNUC__) && (__GNUC_MINOR__ < 20))

//  There's a bug in g++3.x on MinGW that makes basic_string assert
//  when assigning from IterT [f, l) where IterT is a position_iterator.
//  This issue is discussed here:
//
//  http://gcc.gnu.org/ml/libstdc++/2002-03/msg00196.html
//
//  Aparently, this bug is only present on MinGW. I'm clueless as
//  to why this is so. Regressions on linux seem to be OK! :(
//
//  With, g++3.1, assigning to basic_string from IterT [f, l) is a
//  compile error (a g++3.1 bug).
//
//  In both cases above, we use a vector instead of a string.

    typedef std::vector<char> store;
#else
    typedef std::string store;
#endif

    store dst;
    rule<scanner_t> r = (*alpha_p)[assign_a(dst)];

    parse(b, e, r);

    store::iterator d = dst.begin();

    while (b != e)
    {
        if (*d != *b)
        BOOST_TEST(*d == *b);
        ++b;
        ++d;
    }
}

void sf_bug_742038()
{
    std::string src = "abcdef";
    const char* tmpfilename = "sf_bug_742038.tmp";

    test_assign(src.begin(), src.end());

    position_iterator<std::string::iterator> b(src.begin(), src.end(), "");
    position_iterator<std::string::iterator> e;
    test_assign(b, e);

    std::fstream f(tmpfilename, std::ios::out);
    f << src;
    f.close();

    file_iterator<> b1(tmpfilename);
    file_iterator<> e1(b1.make_end());
    test_assign(b1, e1);

    ::remove(tmpfilename);
}

int
test_main(int, char *[])
{
    bug_001();
    bug_002();
    bug_003();
    bug_004();
    bug_005();
    bug_006();
    bug_007();

    sf_bug_718903();
    sf_bug_719322();
    sf_bug_742038();

    return 0;
}
