/*=============================================================================
    Copyright (c) 2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <iostream>
#include <cassert>

using namespace std;

#define BOOST_SPIRIT_SWITCH_CASE_LIMIT 6
#define BOOST_SPIRIT_SELECT_LIMIT 6
#define PHOENIX_LIMIT 6

//#define BOOST_SPIRIT_DEBUG
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>

#include <boost/spirit/core/primitives/primitives.hpp>
#include <boost/spirit/core/primitives/numerics.hpp>
#include <boost/spirit/core/composite/actions.hpp>
#include <boost/spirit/core/composite/operators.hpp>
#include <boost/spirit/core/non_terminal/rule.hpp>
#include <boost/spirit/core/non_terminal/grammar.hpp>
#include <boost/spirit/dynamic/switch.hpp>
#include <boost/spirit/dynamic/select.hpp>
#include <boost/spirit/attribute/closure.hpp>

using namespace boost::spirit;

namespace test_grammars {

///////////////////////////////////////////////////////////////////////////////
//  Test the direct switch_p usage
    struct switch_grammar_direct_single 
    :   public grammar<switch_grammar_direct_single>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_direct_single const& /*self*/)
            {
                r = switch_p [
                        case_p<'a'>(int_p)
                    ];
            }

            rule<ScannerT> r;
            rule<ScannerT> const& start() const { return r; }
        };
    };

    struct switch_grammar_direct_default_single1 
    :   public grammar<switch_grammar_direct_default_single1>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_direct_default_single1 const& /*self*/)
            {
                r = switch_p [
                        default_p(str_p("default"))
                    ];
            }

            rule<ScannerT> r;
            rule<ScannerT> const& start() const { return r; }
        };
    };

    struct switch_grammar_direct_default_single2 
    :   public grammar<switch_grammar_direct_default_single2>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_direct_default_single2 const& /*self*/)
            {
                r = switch_p [
                        default_p
                    ];
            }

            rule<ScannerT> r;
            rule<ScannerT> const& start() const { return r; }
        };
    };

///////////////////////////////////////////////////////////////////////////////
//  Test the switch_p usage given a parser as the switch condition
    struct switch_grammar_parser_single 
    :   public grammar<switch_grammar_parser_single>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_parser_single const& /*self*/)
            {
                r = switch_p(anychar_p) [
                        case_p<'a'>(int_p)
                    ];
            }

            rule<ScannerT> r;
            rule<ScannerT> const& start() const { return r; }
        };
    };

    struct switch_grammar_parser_default_single1 
    :   public grammar<switch_grammar_parser_default_single1>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_parser_default_single1 const& /*self*/)
            {
                r = switch_p(anychar_p) [
                        default_p(str_p("default"))
                    ];
            }

            rule<ScannerT> r;
            rule<ScannerT> const& start() const { return r; }
        };
    };

    struct switch_grammar_parser_default_single2 
    :   public grammar<switch_grammar_parser_default_single2>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_parser_default_single2 const& /*self*/)
            {
                r = switch_p(anychar_p) [
                        default_p
                    ];
            }

            rule<ScannerT> r;
            rule<ScannerT> const& start() const { return r; }
        };
    };

///////////////////////////////////////////////////////////////////////////////
//  Test the switch_p usage given an actor as the switch condition
    struct select_result : public boost::spirit::closure<select_result, int>
    {
        member1 val;
    };

    struct switch_grammar_actor_single 
    :   public grammar<switch_grammar_actor_single>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_actor_single const& /*self*/)
            {
                using phoenix::arg1;
                r = select_p('a')[r.val = arg1] >>
                    switch_p(r.val) [
                        case_p<0>(int_p)
                    ];
            }

            rule<ScannerT, select_result::context_t> r;
            rule<ScannerT, select_result::context_t> const& 
            start() const { return r; }
        };
    };

    struct switch_grammar_actor_default_single1 
    :   public grammar<switch_grammar_actor_default_single1>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_actor_default_single1 const& /*self*/)
            {
                using phoenix::arg1;
                r = select_p('d')[r.val = arg1] >>
                    switch_p(r.val) [
                        default_p(str_p("default"))
                    ];
            }

            rule<ScannerT, select_result::context_t> r;
            rule<ScannerT, select_result::context_t> const& 
            start() const { return r; }
        };
    };

    struct switch_grammar_actor_default_single2 
    :   public grammar<switch_grammar_actor_default_single2>
    {
        template <typename ScannerT>
        struct definition 
        {
            definition(switch_grammar_actor_default_single2 const& /*self*/)
            {
                using phoenix::arg1;
                r = select_p('d')[r.val = arg1] >>
                    switch_p(r.val) [
                        default_p
                    ];
            }

            rule<ScannerT, select_result::context_t> r;
            rule<ScannerT, select_result::context_t> const& 
            start() const { return r; }
        };
    };
    
}   // namespace test_grammars

///////////////////////////////////////////////////////////////////////////////
namespace tests {

    //  Tests for known (to the grammars) sequences
    struct check_grammar_unknown {
    
        template <typename GrammarT>
        void operator()(GrammarT)
        {
            GrammarT g;
            
            assert(!parse("a1", g).hit);
            assert(!parse("a,", g).hit);
            assert(!parse("abcd", g).hit);
            
            assert(!parse("a 1", g, space_p).hit);
            assert(!parse("a ,", g, space_p).hit);
            assert(!parse("a bcd", g, space_p).hit);
            
            assert(!parse("b1", g).hit);
            assert(!parse("b,", g).hit);
            assert(!parse("bbcd", g).hit);
            
            assert(!parse("b 1", g, space_p).hit);
            assert(!parse("b ,", g, space_p).hit);
            assert(!parse("b bcd", g, space_p).hit);
            
            assert(!parse("c1", g).hit);
            assert(!parse("c,", g).hit);
            assert(!parse("cbcd", g).hit);
            
            assert(!parse("c 1", g, space_p).hit);
            assert(!parse("c ,", g, space_p).hit);
            assert(!parse("c bcd", g, space_p).hit);
        }
    };

    //  Tests for the default branches (with parsers) of the grammars
    struct check_grammar_default {
    
        template <typename GrammarT>
        void operator()(GrammarT)
        {
            GrammarT g;
            
            assert(parse("ddefault", g).full);
            assert(parse("d default", g, space_p).full);
        }
    };
    
    //  Tests for the default branches (without parsers) of the grammars
    struct check_grammar_default_plain {
    
        template <typename GrammarT>
        void operator()(GrammarT)
        {
            GrammarT g;
            
            assert(parse("d", g).full);
            assert(parse("d ", g, space_p).full);
        }
    };
    
    //  Tests grammars with a single case_p branch
    struct check_grammar_single {
    
        template <typename GrammarT>
        void operator()(GrammarT)
        {
            GrammarT g;

            assert(parse("a1", g).full);
            assert(!parse("a,", g).hit);
            assert(!parse("abcd", g).hit);

            assert(parse("a 1", g, space_p).full);
            assert(!parse("a ,", g, space_p).hit);
            assert(!parse("a bcd", g, space_p).hit);

            assert(!parse("b1", g).hit);
            assert(!parse("b,", g).hit);
            assert(!parse("bbcd", g).hit);
            
            assert(!parse("b 1", g, space_p).hit);
            assert(!parse("b ,", g, space_p).hit);
            assert(!parse("b bcd", g, space_p).hit);
            
            assert(!parse("c1", g).hit);
            assert(!parse("c,", g).hit);
            assert(!parse("cbcd", g).hit);
            
            assert(!parse("c 1", g, space_p).hit);
            assert(!parse("c ,", g, space_p).hit);
            assert(!parse("c bcd", g, space_p).hit);
        }
    };
    
}   // namespace tests

int 
main()
{
    // Test switch_p with a single case_p branch
    typedef boost::mpl::list<
        test_grammars::switch_grammar_direct_single,
        test_grammars::switch_grammar_parser_single,
        test_grammars::switch_grammar_actor_single
    > single_list_t;
    
    boost::mpl::for_each<single_list_t>(tests::check_grammar_single());
    
    typedef boost::mpl::list<
        test_grammars::switch_grammar_direct_default_single1,
        test_grammars::switch_grammar_parser_default_single1,
        test_grammars::switch_grammar_actor_default_single1
    > default_single_t;
    
    boost::mpl::for_each<default_single_t>(tests::check_grammar_default());
    boost::mpl::for_each<default_single_t>(tests::check_grammar_unknown());
    
    typedef boost::mpl::list<
        test_grammars::switch_grammar_direct_default_single2,
        test_grammars::switch_grammar_parser_default_single2,
        test_grammars::switch_grammar_actor_default_single2
    > default_plain_single_t;
    
    boost::mpl::for_each<default_plain_single_t>(
        tests::check_grammar_default_plain());
    
    return 0;
}
