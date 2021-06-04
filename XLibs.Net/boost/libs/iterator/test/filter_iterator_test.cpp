// Copyright David Abrahams 2003, Jeremy Siek 2004. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/iterator/new_iterator_tests.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/concept_check.hpp>
#include <boost/concept_archetype.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/iterator/iterator_archetypes.hpp>
#include <boost/cstdlib.hpp>

#include <deque>
#include <iostream>

using boost::dummyT;
    
struct one_or_four
{
    bool operator()(dummyT x) const
    {
        return x.foo() == 1 || x.foo() == 4;
    }
};

template <class T> struct undefined;

template <class T> struct see_type;

// Test filter iterator
int main()
{
    // Concept checks
    // Adapting old-style iterators
    {
      typedef boost::filter_iterator<one_or_four, boost::input_iterator_archetype<dummyT> > Iter;
      boost::function_requires< boost::InputIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::SinglePassIteratorConcept<Iter> >();
    }
    {
      typedef boost::filter_iterator<one_or_four, boost::input_output_iterator_archetype<dummyT> > Iter;
      boost::function_requires< boost::InputIteratorConcept<Iter> >();
      boost::function_requires< boost::OutputIteratorConcept<Iter, dummyT> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::WritableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::SinglePassIteratorConcept<Iter> >();
    }
    {
      typedef boost::filter_iterator<one_or_four, boost::forward_iterator_archetype<dummyT> > Iter;
      boost::function_requires< boost::ForwardIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ForwardTraversalConcept<Iter> >();
    }
    {
      typedef boost::filter_iterator<one_or_four, boost::mutable_forward_iterator_archetype<dummyT> > Iter;
      boost::function_requires< boost::Mutable_ForwardIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::WritableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ForwardTraversalConcept<Iter> >();
    }
    // Adapting new-style iterators
    {
      typedef boost::iterator_archetype<
          const dummyT
        , boost::iterator_archetypes::readable_iterator_t
        , boost::single_pass_traversal_tag
      > BaseIter;
      typedef boost::filter_iterator<one_or_four, BaseIter> Iter;
      boost::function_requires< boost::InputIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::SinglePassIteratorConcept<Iter> >();
    }
#if !BOOST_WORKAROUND(BOOST_MSVC, == 1200)  // Causes Internal Error in linker.
    {
      typedef boost::iterator_archetype<
          dummyT
        , boost::iterator_archetypes::readable_writable_iterator_t
        , boost::single_pass_traversal_tag
      > BaseIter;
      typedef boost::filter_iterator<one_or_four, BaseIter> Iter;

      boost::function_requires< boost::InputIteratorConcept<Iter> >();
      boost::function_requires< boost::OutputIteratorConcept<Iter, dummyT> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::WritableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::SinglePassIteratorConcept<Iter> >();
    }
#endif 
    {
      typedef boost::iterator_archetype<
          const dummyT
        , boost::iterator_archetypes::readable_iterator_t
        , boost::forward_traversal_tag
      > BaseIter;
      typedef boost::filter_iterator<one_or_four, BaseIter> Iter;
      boost::function_requires< boost::InputIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ForwardTraversalConcept<Iter> >();
    }
    
#if !BOOST_WORKAROUND(BOOST_MSVC, == 1200)  // Causes Internal Error in linker.
    {
      typedef boost::iterator_archetype<
          dummyT
        , boost::iterator_archetypes::readable_writable_iterator_t
        , boost::forward_traversal_tag
      > BaseIter;
      typedef boost::filter_iterator<one_or_four, BaseIter> Iter;
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::WritableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ForwardTraversalConcept<Iter> >();
    }
    {
      typedef boost::iterator_archetype<
          const dummyT
        , boost::iterator_archetypes::readable_lvalue_iterator_t
        , boost::forward_traversal_tag
      > BaseIter;
      typedef boost::filter_iterator<one_or_four, BaseIter> Iter;
      boost::function_requires< boost::ForwardIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ReadableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::LvalueIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ForwardTraversalConcept<Iter> >();
    }
    {
      typedef boost::iterator_archetype<
          dummyT
        , boost::iterator_archetypes::writable_lvalue_iterator_t
        , boost::forward_traversal_tag
      > BaseIter;
      typedef boost::filter_iterator<one_or_four, BaseIter> Iter;
      boost::function_requires< boost::Mutable_ForwardIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::WritableIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::LvalueIteratorConcept<Iter> >();
      boost::function_requires< boost_concepts::ForwardTraversalConcept<Iter> >();
    }
#endif 

    // Run-time tests

    dummyT array[] = { dummyT(0), dummyT(1), dummyT(2), 
                       dummyT(3), dummyT(4), dummyT(5) };
    const int N = sizeof(array)/sizeof(dummyT);

    typedef boost::filter_iterator<one_or_four, dummyT*> filter_iter;

    boost::bidirectional_readable_iterator_test(
        filter_iter(one_or_four(), array, array+N)
        , dummyT(1), dummyT(4));

    BOOST_STATIC_ASSERT(
        (!boost::is_convertible<
             boost::iterator_traversal<filter_iter>::type
           , boost::random_access_traversal_tag
         >::value
        ));
    
    //# endif
    
    // On compilers not supporting partial specialization, we can do more type
    // deduction with deque iterators than with pointers... unless the library
    // is broken ;-(
    std::deque<dummyT> array2;
    std::copy(array+0, array+N, std::back_inserter(array2));
    boost::bidirectional_readable_iterator_test(
        boost::make_filter_iterator(one_or_four(), array2.begin(), array2.end()),
        dummyT(1), dummyT(4));

    boost::bidirectional_readable_iterator_test(
        boost::make_filter_iterator(one_or_four(), array2.begin(), array2.end()),
        dummyT(1), dummyT(4));

    boost::bidirectional_readable_iterator_test(
        boost::make_filter_iterator(
              one_or_four()
            , boost::make_reverse_iterator(array2.end())
            , boost::make_reverse_iterator(array2.begin())
            ),
        dummyT(4), dummyT(1));
    
    boost::bidirectional_readable_iterator_test(
        filter_iter(array+0, array+N),
        dummyT(1), dummyT(4));

    boost::bidirectional_readable_iterator_test(
        filter_iter(one_or_four(), array, array + N),
        dummyT(1), dummyT(4));


  std::cout << "test successful " << std::endl;
  return boost::exit_success;
}
