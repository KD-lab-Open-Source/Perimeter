+++++++++++++++++++++++++++++++++++++
 Iterator concept and adapter issues 
+++++++++++++++++++++++++++++++++++++

:date: $Date: 2004/01/27 03:17:30 $

.. contents:: Index

===================================
 Issues from Matt's TR issues list
===================================


9.1 iterator_access overspecified?
==================================

:Submitter: Pete Becker 
:Status: New 

The proposal includes::

  enum iterator_access { 
     readable_iterator = 1, writable_iterator = 2,
     swappable_iterator = 4, lvalue_iterator = 8
  }; 

In general, the standard specifies thing like this as a bitmask
type with a list of defined names, and specifies neither the exact
type nor the specific values. Is there a reason for iterator_access
to be more specific?

:Proposed resolution: The ``iterator_access`` enum will be removed,
   so this is no longer an issue.  See the resolution to 9.15.


9.2 operators of iterator_facade overspecified 
==============================================

:Submitter: Pete Becker 
:Status: New 

In general, we've provided operational semantics for things like
operator++. That is, we've said that ++iter must work, without
requiring either a member function or a non-member function.
iterator_facade specifies most operators as member
functions. There's no inherent reason for these to be members, so
we should remove this requirement. Similarly, some operations are
specified as non-member functions but could be implemented as
members. Again, the standard doesn't make either of these choices,
and TR1 shouldn't, either. So: ``operator*()``, ``operator++()``,
``operator++(int)``, ``operator--()``, ``operator--(int)``,
``operator+=``, ``operator-=``, ``operator-(difference_type)``,
``operator-(iterator_facade instance)``, and ``operator+`` should
be specified with operational semantics and not explicitly required
to be members or non-members.

:Proposed resolution: Not a defect. 

:Rationale: The standard uses valid expressions such as ``++iter``
  in requirements tables, such as for input iterator.  However, for
  classes, such as ``reverse_iterator``, the standard uses function
  prototypes, as we have done here for
  ``iterator_facade``. Further, the prototype specification does
  not prevent the implementor from using members or non-members,
  since nothing the user can do in a conforming program can detect
  how the function is implemented.


9.3 enable_if_interoperable needs standardese
=============================================

:Submitter: Pete Becker 
:Status: New 

The only discussion of what this means is in a note, so is
non-normative. Further, the note seems to be incorrect. It says
that enable_if_interoperable only works for types that "are
interoperable, by which we mean they are convertible to each
other." This requirement is too strong: it should be that one of
the types is convertible to the other.  N1541 48

:Proposed resolution: Add normative text. Relax requirements in the
  proposed way.

  Change:

    [*Note:* The ``enable_if_interoperable`` template used above is
    for exposition purposes. The member operators should be only be
    in an overload set provided the derived types ``Dr1`` and
    ``Dr2`` are interoperable, by which we mean they are
    convertible to each other.  The ``enable_if_interoperable``
    approach uses SFINAE to take the operators out of the overload
    set when the types are not interoperable.]

  To:

    The ``enable_if_interoperable`` template used above is for
    exposition purposes.  The member operators should only be in an
    overload set provided the derived types ``Dr1`` and ``Dr2`` are
    interoperable, meaning that at least one of the types is
    convertible to the other.  The ``enable_if_interoperable``
    approach uses SFINAE to take the operators out of the overload
    set when the types are not interoperable.  The operators should
    behave *as-if* ``enable_if_interoperable`` were defined to be::
    
      template <bool, typename> enable_if_interoperable_impl
      {};
    
      template <typename T> enable_if_interoperable_impl<true,T>
      { typedef T type; };
    
      template<typename Dr1, typename Dr2, typename T>
      struct enable_if_interoperable
        : enable_if_interoperable_impl<
              is_convertible<Dr1,Dr2>::value || is_convertible<Dr2,Dr1>::value
            , T
          >
      {};

9.4 enable_if_convertible unspecified, conflicts with requires 
==============================================================

:Submitter: Pete Becker 
:Status: New 

In every place where enable_if_convertible is used it's used like
this (simplified)::

  template<class T>
  struct C
  {
    template<class T1>
    C(T1, enable_if_convertible<T1, T>::type* = 0);
  };

The idea being that this constructor won't compile if T1 isn't
convertible to T. As a result, the constructor won't be considered
as a possible overload when constructing from an object x where the
type of x isn't convertible to T. In addition, however, each of
these constructors has a requires clause that requires
convertibility, so the behavior of a program that attempts such a
construction is undefined. Seems like the enable_if_convertible
part is irrelevant, and should be removed.  There are two
problems. First, enable_if_convertible is never specified, so we
don't know what this is supposed to do. Second: we could reasonably
say that this overload should be disabled in certain cases or we
could reasonably say that behavior is undefined, but we can't say
both.

Thomas Witt writes that the goal of putting in
enable_if_convertible here is to make sure that a specific overload
doesn't interfere with the generic case except when that overload
makes sense. He agrees that what we currently have is deficient.
Dave Abrahams writes that there is no conflict with the requires
cause because the requires clause only takes effect when the
function is actually called. The presence of the constructor
signature can/will be detected by is_convertible without violating
the requires clause, and thus it makes a difference to disable
those constructor instantiations that would be disabled by
enable_if_convertible even if calling them invokes undefined
behavior.  There was more discussion on the reflector:
c++std-lib-12312, c++std-lib-12325, c++std-lib- 12330,
c++std-lib-12334, c++std-lib-12335, c++std-lib-12336,
c++std-lib-12338, c++std-lib- 12362.

:Proposed resolution: 
  Change:

    [*Note:* The ``enable_if_convertible<X,Y>::type`` expression
    used in this section is for exposition purposes. The converting
    constructors for specialized adaptors should be only be in an
    overload set provided that an object of type ``X`` is
    implicitly convertible to an object of type ``Y``.  The
    ``enable_if_convertible`` approach uses SFINAE to take the
    constructor out of the overload set when the types are not
    implicitly convertible.]
    
  To:

    The ``enable_if_convertible<X,Y>::type`` expression used in
    this section is for exposition purposes. The converting
    constructors for specialized adaptors should be only be in an
    overload set provided that an object of type ``X`` is
    implicitly convertible to an object of type ``Y``.  The
    signatures involving ``enable_if_convertible`` should behave
    *as-if* ``enable_if_convertible`` were defined to be::

      template <bool> enable_if_convertible_impl
      {};

      template <> enable_if_convertible_impl<true>
      { struct type; };

      template<typename From, typename To>
      struct enable_if_convertible
        : enable_if_convertible_impl<is_convertible<From,To>::value>
      {};

    If an expression other than the default argument is used to
    supply the value of a function parameter whose type is written
    in terms of ``enable_if_convertible``, the program is
    ill-formed, no diagnostic required.

    [*Note:* The ``enable_if_convertible`` approach uses SFINAE to
    take the constructor out of the overload set when the types are
    not implicitly convertible.  ]

9.5 iterator_adaptor has an extraneous 'bool' at the start of the template definition 
=====================================================================================

:Submitter: Pete Becker 
:Status: New 

The title says it all; this is probably just a typo. 

:Proposed resolution: Remove the 'bool'.

9.6 Name of private member shouldn't be normative 
=================================================

:Submitter: Pete Becker 
:Status: New 

iterator_adaptor has a private member named m_iterator. Presumably
this is for exposition only, since it's an implementation
detail. It needs to be marked as such.

:Proposed resolution: Mark the member ``m_iterator`` as exposition
   only.  Note/DWA: I think this is NAD because the user can't
   detect it, though I'm happy to mark it exposition only.

  In [lib.iterator.adaptor]

  Change::

    Base m_iterator;

  to::

    Base m_iterator; // exposition only


9.7 iterator_adaptor operations specifications are a bit inconsistent 
=====================================================================

:Submitter: Pete Becker 
:Status: New 

iterator_adpator() has a Requires clause, that Base must be default
constructible.  iterator_adaptor(Base) has no Requires clause,
although the Returns clause says that the Base member is copy
construced from the argument (this may actually be an oversight in
N1550, which doesn't require iterators to be copy constructible or
assignable).

:Proposed resolution: Add a requirements section for the template
  parameters of iterator_adaptor, and state that Base must be Copy
  Constructible and Assignable.

  N1550 does in fact include requirements for copy constructible
  and assignable in the requirements tables. To clarify, we've also
  added the requirements to the text.


9.8 Specialized adaptors text should be normative 
=================================================

:Submitter: Pete Becker 
:Status: New 

similar to 9.3, "Specialized Adaptors" has a note describing
enable_if_convertible. This should be normative text.

:Proposed resolution: Changed it to normative
  text.  See the resolution of 9.4

9.9 Reverse_iterator text is too informal 
=========================================

:Submitter: Pete Becker 
:Status: New 

reverse iterator "flips the direction of the base iterator's
motion". This needs to be more formal, as in the current
standard. Something like: "iterates through the controlled sequence
in the opposite direction"

:Proposed resolution:

  Change:

    The reverse iterator adaptor flips the direction of a base
    iterator's motion. Invoking ``operator++()`` moves the base
    iterator backward and invoking ``operator--()`` moves the base
    iterator forward.

  to:

    The reverse iterator adaptor iterates through the adapted iterator
    range in the opposite direction.


9.10 'prior' is undefined 
=========================

:Submitter: Pete Becker 
:Status: New 

reverse_iterator::dereference is specified as calling a function
named 'prior' which has no specification.

:Proposed resolution:
  Change the specification to avoid using ``prior`` as follows.

  Remove::

    typename reverse_iterator::reference dereference() const { return *prior(this->base()); }

  And at the end of the operations section add:

    ``reference operator*() const;``

    :Effects: 

    ::

        Iterator tmp = m_iterator;
        return *--tmp;

:Rationale:
  The style of specification has changed because of issue 9.37x.



9.11 "In other words" is bad wording 
====================================

:Submitter: Pete Becker 
:Status: New 

Transform iterator has a two-part specification: it does this, in
other words, it does that. "In other words" always means "I didn't
say it right, so I'll try again." We need to say it once.

:Proposed resolution:
  Change:

    The transform iterator adapts an iterator by applying some function
    object to the result of dereferencing the iterator. In other words,
    the ``operator*`` of the transform iterator first dereferences the
    base iterator, passes the result of this to the function object, and
    then returns the result.

  to:

    The transform iterator adapts an iterator by modifying the
    ``operator*`` to apply a function object to the result of
    dereferencing the iterator and returning the result.


9.12 Transform_iterator shouldn't mandate private member 
========================================================

:Submitter: Pete Becker 
:Status: New 

transform_iterator has a private member named 'm_f' which should be
marked "exposition only."

:Proposed resolution: Mark the member ``m_f`` as exposition
  only. Note/DWA: I think this is NAD because the user can't
  detect it, though I'm happy to mark it exposition only.

  Change::

    UnaryFunction m_f;

  to::

    UnaryFunction m_f;   // exposition only



9.13 Unclear description of counting iterator 
=============================================

:Submitter: Pete Becker 
:Status: New 

The description of Counting iterator is unclear. "The counting
iterator adaptor implements dereference by returning a reference to
the base object. The other operations are implemented by the base
m_iterator, as per the inheritance from iterator_adaptor."

:Proposed resolution:
  Change:

    The counting iterator adaptor implements dereference by
    returning a reference to the base object. The other operations
    are implemented by the base ``m_iterator``, as per the
    inheritance from ``iterator_adaptor``.

  to:

    ``counting_iterator`` adapts an object by adding an
    ``operator*`` that returns the current value of the object. All
    other iterator operations are forwarded to the adapted object.



9.14 Counting_iterator's difference type 
========================================

:Submitter: Pete Becker 
:Status: New 

Counting iterator has the following note: 

  [Note: implementers are encouraged to provide an implementation
  of distance_to and a difference_type that avoids overflows in the
  cases when the Incrementable type is a numeric type.]

I'm not sure what this means. The user provides a template argument
named Difference, but there's no difference_type. I assume this is
just a glitch in the wording. But if implementors are encouraged to
ignore this argument if it won't work right, why is it there?

:Proposed resolution: The ``difference_type`` was inherited from
  ``iterator_adaptor``.  However, we've removed the explicit
  inheritance, so explicit typedefs have been added. See the
  resolution of 9.37x.



9.15 How to detect lvalueness? 
==============================

:Submitter: Dave Abrahams 
:Status: New 

Shortly after N1550 was accepted, we discovered that an iterator's
lvalueness can be determined knowing only its value_type. This
predicate can be calculated even for old-style iterators (on whose
reference type the standard places few requirements). A trait in
the Boost iterator library does it by relying on the compiler's
unwillingness to bind an rvalue to a T& function template
parameter. Similarly, it is possible to detect an iterator's
readability knowing only its value_type.  Thus, any interface which
asks the user to explicitly describe an iterator's lvalue-ness or
readability seems to introduce needless complexity.


:Proposed resolution: 

  1. Remove the ``is_writable`` and ``is_swappable`` traits, and
     remove the requirements in the Writable Iterator and Swappable
     Iterator concepts that require their models to support these
     traits.

  2. Change the ``is_readable`` specification.  Remove the
     requirement for support of the ``is_readable`` trait from the
     Readable Iterator concept.

  3. Remove the ``iterator_tag`` class and transplant the logic for
     choosing an iterator category into ``iterator_facade``.

  4. Change the specification of ``traversal_category``.

  5. Remove Access parameters from N1530

  In N1550:

  Remove:

    Since the access concepts are not related via refinement, but
    instead cover orthogonal issues, we do not use tags for the
    access concepts, but instead use the equivalent of a bit field.
    
    We provide an access mechanism for mapping iterator types to
    the new traversal tags and access bit field. Our design reuses
    ``iterator_traits<Iter>::iterator_category`` as the access
    mechanism. To that end, the access and traversal information is
    bundled into a single type using the following `iterator_tag`
    class.
    
    ::
    
      enum iterator_access { readable_iterator = 1, writable_iterator = 2, 
          swappable_iterator = 4, lvalue_iterator = 8 };
    
      template <unsigned int access_bits, class TraversalTag>
      struct iterator_tag : /* appropriate old category or categories */ {
        static const iterator_access access =
          (iterator_access)access_bits & 
            (readable_iterator | writable_iterator | swappable_iterator);
        typedef TraversalTag traversal;
      };
    
    The ``access_bits`` argument is declared to be ``unsigned int``
    instead of the enum ``iterator_access`` for convenience of
    use. For example, the expression ``(readable_iterator |
    writable_iterator)`` produces an unsigned int, not an
    ``iterator_access``.  The purpose of the ``lvalue_iterator``
    part of the ``iterator_access`` enum is to communicate to
    ``iterator_tag`` whether the reference type is an lvalue so
    that the appropriate old category can be chosen for the base
    class. The ``lvalue_iterator`` bit is not recorded in the
    ``iterator_tag::access`` data member.
    
    The ``iterator_tag`` class template is derived from the
    appropriate iterator tag or tags from the old requirements
    based on the access bits and traversal tag passed as template
    parameters.  The algorithm for determining the old tag or tags
    picks the least refined old concepts that include all of the
    requirements of the access and traversal concepts (that is, the
    closest fit), if any such category exists.  For example, the
    category tag for a Readable Single Pass Iterator will always be
    derived from ``input_iterator_tag``, while the category tag for
    a Single Pass Iterator that is both Readable and Writable will
    be derived from both ``input_iterator_tag`` and
    ``output_iterator_tag``.
    
    We also provide several helper classes that make it convenient
    to obtain the access and traversal characteristics of an
    iterator. These helper classes work both for iterators whose
    ``iterator_category`` is ``iterator_tag`` and also for
    iterators using the original iterator categories.
    
    ::
    
      template <class Iterator> struct is_readable  { typedef ... type; };
      template <class Iterator> struct is_writable { typedef ... type; };
      template <class Iterator> struct is_swappable { typedef ... type; };
      template <class Iterator> struct traversal_category { typedef ... type; };


  After:

    Like the old iterator requirements, we provide tags for
    purposes of dispatching based on the traversal concepts.  The
    tags are related via inheritance so that a tag is convertible
    to another tag if the concept associated with the first tag is
    a refinement of the second tag.

  Add:

    Our design reuses ``iterator_traits<Iter>::iterator_category``
    to indicate an iterator's traversal capability.  To specify
    capabilities not captured by any old-style iterator category,
    an iterator designer can use an ``iterator_category`` type that
    is convertible to both the the most-derived old iterator
    category tag which fits, and the appropriate new iterator
    traversal tag.

    We do not provide tags for the purposes of dispatching based on
    the access concepts, in part because we could not find a way to
    automatically infer the right access tags for old-style
    iterators.  An iterator's writability may be dependent on the
    assignability of its ``value_type`` and there's no known way to
    detect whether an arbitrary type is assignable.  Fortunately,
    the need for dispatching based on access capability is not as
    great as the need for dispatching based on traversal
    capability.


  From the Readable Iterator Requirements table, remove:

    +-----------------------------------+-----------------------------------+-------------------------+
    |``is_readable<X>::type``           |``true_type``                      |                         |
    +-----------------------------------+-----------------------------------+-------------------------+

  From the Writable Iterator Requirements table, remove:

    +-------------------------+--------------+----------------------------+
    |``is_writable<X>::type`` |``true_type`` |                            |
    +-------------------------+--------------+----------------------------+

  From the Swappable Iterator Requirements table, remove:

    +-------------------------+-------------+-----------------------------+
    |``is_swappable<X>::type``|``true_type``|                             |
    +-------------------------+-------------+-----------------------------+


  From [lib.iterator.synopsis] replace::

    template <class Iterator> struct is_readable;
    template <class Iterator> struct is_writable;
    template <class Iterator> struct is_swappable;
    template <class Iterator> struct traversal_category;
  
    enum iterator_access { readable_iterator = 1, writable_iterator = 2, 
        swappable_iterator = 4, lvalue_iterator = 8 };
  
    template <unsigned int access_bits, class TraversalTag>
    struct iterator_tag : /* appropriate old category or categories */ {
      static const iterator_access access =
        (iterator_access)access_bits & 
          (readable_iterator | writable_iterator | swappable_iterator);
      typedef TraversalTag traversal;
    };

  with::

    template <class Iterator> struct is_readable_iterator;
    template <class Iterator> struct iterator_traversal;


  In [lib.iterator.traits], remove:

    The ``iterator_tag`` class template is an iterator category tag
    that encodes the access enum and traversal tag in addition to
    being compatible with the original iterator tags.  The
    ``iterator_tag`` class inherits from one of the original
    iterator tags according to the following pseudo-code.
    
    ::
     
        inherit-category(access, traversal-tag) =
             if ((access & readable_iterator) && (access & lvalue_iterator)) {
                 if (traversal-tag is convertible to random_access_traversal_tag)
                     return random_access_iterator_tag;
                 else if (traversal-tag is convertible to bidirectional_traversal_tag)
                     return bidirectional_iterator_tag;
                 else if (traversal-tag is convertible to forward_traversal_tag)
                     return forward_iterator_tag;
                 else if (traversal-tag is convertible to single_pass_traversal_tag)
                     if (access-tag is convertible to writable_iterator_tag)
                         return input_output_iterator_tag;
                     else
                         return input_iterator_tag;
                 else
                     return null_category_tag;
             } else if ((access & readable_iterator) and (access & writable_iterator)
                        and traversal-tag is convertible to single_pass_iterator_tag)
                 return input_output_iterator_tag;
             else if (access & readable_iterator
                      and traversal-tag is convertible to single_pass_iterator_tag)
                 return input_iterator_tag;
             else if (access & writable_iterator
                      and traversal-tag is convertible to incrementable_iterator_tag)
                 return output_iterator_tag;
             else
                 return null_category_tag;

    If the argument for ``TraversalTag`` is not convertible to
    ``incrementable_iterator_tag`` then the program is ill-formed.

  Change:

    The ``is_readable``, ``is_writable``, ``is_swappable``, and
    ``traversal_category`` class templates are traits classes. For
    iterators whose ``iterator_traits<Iter>::iterator_category``
    type is ``iterator_tag``, these traits obtain the ``access``
    enum and ``traversal`` member type from within
    ``iterator_tag``.  For iterators whose
    ``iterator_traits<Iter>::iterator_category`` type is not
    ``iterator_tag`` and instead is a tag convertible to one of the
    original tags, the appropriate traversal tag and access bits
    are deduced.  The following pseudo-code describes the
    algorithm.

    ::

      is-readable(Iterator) = 
          cat = iterator_traits<Iterator>::iterator_category;
          if (cat == iterator_tag<Access,Traversal>)
              return Access & readable_iterator;
          else if (cat is convertible to input_iterator_tag)
              return true;
          else
              return false;

      is-writable(Iterator) =
          cat = iterator_traits<Iterator>::iterator_category;
          if (cat == iterator_tag<Access,Traversal>)
              return Access & writable_iterator;
          else if (cat is convertible to output_iterator_tag)
               return true;
          else if (
               cat is convertible to forward_iterator_tag
               and iterator_traits<Iterator>::reference is a 
                   mutable reference)
              return true;
          else
              return false;

      is-swappable(Iterator) =
          cat = iterator_traits<Iterator>::iterator_category;
          if (cat == iterator_tag<Access,Traversal>)
              return Access & swappable_iterator;
          else if (cat is convertible to forward_iterator_tag) {
              if (iterator_traits<Iterator>::reference is a const reference)
                  return false;
              else
                  return true;
          } else 
              return false;

      traversal-category(Iterator) =
          cat = iterator_traits<Iterator>::iterator_category;
          if (cat == iterator_tag<Access,Traversal>)
              return Traversal;
          else if (cat is convertible to random_access_iterator_tag)
              return random_access_traversal_tag;
          else if (cat is convertible to bidirectional_iterator_tag)
              return bidirectional_traversal_tag;
          else if (cat is convertible to forward_iterator_tag)
              return forward_traversal_tag;
          else if (cat is convertible to input_iterator_tag)
              return single_pass_iterator_tag;
          else if (cat is convertible to output_iterator_tag)
              return incrementable_iterator_tag;
          else
              return null_category_tag;


    The following specializations provide the access and traversal
    category tags for pointer types.

    ::

      template <typename T>
      struct is_readable<const T*> { typedef true_type type; };
      template <typename T>
      struct is_writable<const T*> { typedef false_type type; };
      template <typename T>
      struct is_swappable<const T*> { typedef false_type type; };

      template <typename T>
      struct is_readable<T*> { typedef true_type type; };
      template <typename T>
      struct is_writable<T*> { typedef true_type type; };
      template <typename T>
      struct is_swappable<T*> { typedef true_type type; };

      template <typename T>
      struct traversal_category<T*>
      {
        typedef random_access_traversal_tag type;
      };

  to:

    The ``is_readable_iterator`` class template satisfies the
    UnaryTypeTrait requirements.

    Given an iterator type ``X``,
    ``is_readable_iterator<X>::value`` yields ``true`` if, for an
    object ``a`` of type ``X``, ``*a`` is convertible to
    ``iterator_traits<X>::value_type``, and ``false`` otherwise.

    .. _`category-to-traversal`: 

    ``iterator_traversal<X>::type`` is 

    .. parsed-literal::

      *category-to-traversal*\ (iterator_traits<X>::iterator_category) 

    where *category-to-traversal* is defined as follows

    .. parsed-literal::

       *category-to-traversal*\ (C) =
           if (C is convertible to incrementable_traversal_tag)
               return C;
           else if (C is convertible to random_access_iterator_tag)
               return random_access_traversal_tag;
           else if (C is convertible to bidirectional_iterator_tag)
               return bidirectional_traversal_tag;
           else if (C is convertible to forward_iterator_tag)
               return forward_traversal_tag;
           else if (C is convertible to input_iterator_tag)
               return single_pass_traversal_tag;
           else if (C is convertible to output_iterator_tag)
               return incrementable_traversal_tag;
           else
               *the program is ill-formed*

  In N1530:

  In [lib.iterator.helper.synopsis]:

  Change::

    const unsigned use_default_access = -1;

    struct iterator_core_access { /* implementation detail */ };

    template <
        class Derived
      , class Value
      , unsigned AccessCategory
      , class TraversalCategory
      , class Reference  = Value&
      , class Difference = ptrdiff_t
    >
    class iterator_facade;

    template <
        class Derived
      , class Base
      , class Value      = use_default
      , unsigned Access  = use_default_access
      , class Traversal  = use_default
      , class Reference  = use_default
      , class Difference = use_default
    >
    class iterator_adaptor;

    template <
        class Iterator
      , class Value = use_default
      , unsigned Access  = use_default_access
      , class Traversal  = use_default
      , class Reference = use_default
      , class Difference = use_default
    >
    class indirect_iterator;

  To::

    struct iterator_core_access { /* implementation detail */ };

    template <
        class Derived
      , class Value
      , class CategoryOrTraversal
      , class Reference  = Value&
      , class Difference = ptrdiff_t
    >
    class iterator_facade;

    template <
        class Derived
      , class Base
      , class Value      = use_default
      , class CategoryOrTraversal  = use_default
      , class Reference  = use_default
      , class Difference = use_default
    >
    class iterator_adaptor;

    template <
        class Iterator
      , class Value = use_default
      , class CategoryOrTraversal = use_default
      , class Reference = use_default
      , class Difference = use_default
    >
    class indirect_iterator;
    
  Change::

    template <
        class Incrementable
      , unsigned Access  = use_default_access
      , class Traversal  = use_default
      , class Difference = use_default
    >
    class counting_iterator

  To::

    template <
        class Incrementable
      , class CategoryOrTraversal  = use_default
      , class Difference = use_default
    >
    class counting_iterator;

  In [lib.iterator.facade]:

  Change::

    template <
        class Derived
      , class Value
      , unsigned AccessCategory
      , class TraversalCategory
      , class Reference  = /* see below */
      , class Difference = ptrdiff_t
    >
    class iterator_facade {

  to::

    template <
        class Derived
      , class Value
      , class CategoryOrTraversal
      , class Reference  = Value&
      , class Difference = ptrdiff_t
    >
    class iterator_facade {


  Change::

        typedef iterator_tag<AccessCategory, TraversalCategory> iterator_category;
  
  to::

        typedef /* see below */ iterator_category;


  Change::
  
    // Comparison operators
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type // exposition
    operator ==(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
                iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator !=(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
                iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator <(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
               iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator <=(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
                iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator >(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
               iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator >=(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
                iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator >=(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
                iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    // Iterator difference
    template <class Dr1, class V1, class AC1, class TC1, class R1, class D1,
              class Dr2, class V2, class AC2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1, Dr2, bool>::type
    operator -(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs,
               iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs);
  
    // Iterator addition
    template <class Derived, class V, class AC, class TC, class R, class D>
    Derived operator+ (iterator_facade<Derived, V, AC, TC, R, D> const&,
                       typename Derived::difference_type n)

  to::

    // Comparison operators
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type // exposition
    operator ==(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator !=(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator <(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
               iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator <=(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator >(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
               iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator >=(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    // Iterator difference
    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    /* see below */
    operator-(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
              iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);
  
    // Iterator addition
    template <class Dr, class V, class TC, class R, class D>
    Derived operator+ (iterator_facade<Dr,V,TC,R,D> const&,
                       typename Derived::difference_type n);
  
    template <class Dr, class V, class TC, class R, class D>
    Derived operator+ (typename Derived::difference_type n,
                       iterator_facade<Dr,V,TC,R,D> const&);


  After the ``iterator_facade`` synopsis, add:

  The ``iterator_category`` member of ``iterator_facade`` is

  .. parsed-literal::

    *iterator-category*\ (CategoryOrTraversal, value_type, reference)

  where *iterator-category* is defined as follows:

  .. parsed-literal::
      
      *iterator-category*\ (C,R,V) :=
         if (C is convertible to std::input_iterator_tag
             || C is convertible to std::output_iterator_tag
         )
             return C
    
         else if (C is not convertible to incrementable_traversal_tag)
             *the program is ill-formed*
    
         else return a type X satisfying the following two constraints:
    
            1. X is convertible to X1, and not to any more-derived
               type, where X1 is defined by:
    
                 if (R is a reference type
                     && C is convertible to forward_traversal_tag)
                 {
                     if (C is convertible to random_access_traversal_tag)
                         X1 = random_access_iterator_tag
                     else if (C is convertible to bidirectional_traversal_tag)
                         X1 = bidirectional_iterator_tag
                     else
                         X1 = forward_iterator_tag
                 }
                 else
                 {
                     if (C is convertible to single_pass_traversal_tag
                         && R is convertible to V)
                         X1 = input_iterator_tag
                     else
                         X1 = C
                 }
    
            2. |category-to-traversal|_\ (X) is convertible to the most
               derived traversal tag type to which X is also
               convertible, and not to any more-derived traversal tag
               type.
    
.. |category-to-traversal| replace:: *category-to-traversal*

.. |iterator-category| replace:: *iterator-category*
.. _iterator-category:

  In [lib.iterator.facade] ``iterator_facade`` requirements:

  Remove:

    ``AccessCategory`` must be an unsigned value which uses no more
    bits than the greatest value of ``iterator_access``.
        
  In the **Iterator Adaptor** section, change:

     Several of the template parameters of ``iterator_adaptor`` default
     to ``use_default`` (or ``use_default_access``). 

  to:

     Several of the template parameters of ``iterator_adaptor`` default
     to ``use_default``. 

  In [lib.iterator.special.adaptors]:

  Change::

    template <
        class Iterator
      , class Value = use_default
      , unsigned Access  = use_default_access
      , class Traversal  = use_default
      , class Reference = use_default
      , class Difference = use_default
    >
    class indirect_iterator

  to::

    template <
        class Iterator
      , class Value = use_default
      , class CategoryOrTraversal = use_default
      , class Reference = use_default
      , class Difference = use_default
    >
    class indirect_iterator

  Change::

    template <
        class Iterator2, class Value2, unsigned Access2, class Traversal2
      , class Reference2, class Difference2
    >
    indirect_iterator(

  to::

    template <
        class Iterator2, class Value2, class Category2
      , class Reference2, class Difference2
    >
    indirect_iterator(

  Change::

    template <
        class Incrementable
      , unsigned Access = use_default_access
      , class Traversal = use_default
      , class Difference = use_default
    >
    class counting_iterator

  to::

    template <
        class Incrementable
      , class CategoryOrTraversal = use_default
      , class Difference = use_default
    >
    class counting_iterator


  Change::

    typedef iterator_tag<
          writable_iterator
        , incrementable_traversal_tag
    > iterator_category;

  to:

    typedef std::output_iterator_tag iterator_category;

  In [lib.iterator.adaptor]

  Change::

    template <
        class Derived
      , class Base
      , class Value      = use_default
      , unsigned Access  = use_default_access
      , class Traversal  = use_default
      , class Reference  = use_default
      , class Difference = use_default
    >
    class iterator_adaptor 

  To::

    template <
        class Derived
      , class Base
      , class Value               = use_default
      , class CategoryOrTraversal = use_default
      , class Reference           = use_default
      , class Difference = use_default
    >
    class iterator_adaptor 
  
:Rationale: 

1. There are two reasons for removing ``is_writable``
   and ``is_swappable``. The first is that we do not know of
   a way to fix the specification so that it gives the correct
   answer for all iterators. Second, there was only a weak
   motivation for having ``is_writable`` and ``is_swappable``
   there in the first place.  The main motivation was simply
   uniformity: we have tags for the old iterator categories
   so we should have tags for the new iterator categories.
   While having tags and the capability to dispatch based
   on the traversal categories is often used, we see
   less of a need for dispatching based on writability
   and swappability, since typically algorithms
   that need these capabilities have no alternative if
   they are not provided.

2. We discovered that the ``is_readable`` trait can be implemented
   using only the iterator type itself and its ``value_type``.
   Therefore we remove the requirement for ``is_readable`` from the
   Readable Iterator concept, and change the definition of
   ``is_readable`` so that it works for any iterator type.

3. The purpose of the ``iterator_tag`` class was to bundle the
   traversal and access category tags into the
   ``iterator_category`` typedef.  With ``is_writable`` and
   ``is_swappable`` gone, and ``is_readable`` no longer in need of
   special hints, there is no reason for iterators to provide
   information about the access capabilities of an iterator.  Thus
   there is no need for the ``iterator_tag``. The traversal tag can
   be directly used for the ``iterator_category``. If a new
   iterator is intended to be backward compatible with old iterator
   concepts, a tag type that is convertible to both one of the new
   traversal tags and also to an old iterator tag can be created
   and use for the ``iterator_category``.

4. The changes to the specification of ``traversal_category`` are a 
   direct result of the removal of ``iterator_tag``.



9.16 is_writable_iterator returns false positives 
=================================================

:Submitter: Dave Abrahams 
:Status: New 

is_writable_iterator returns false positives for forward iterators
whose value_type has a private assignment operator, or whose
reference type is not a reference (currently legal).

:Proposed Resolution: See the resolution to 9.15.


9.17 is_swappable_iterator returns false positives 
==================================================

:Submitter: Dave Abrahams 
:Status: New 

is_swappable_iterator has the same problems as
is_writable_iterator. In addition, if we allow users to write their
own iter_swap functions it's easy to imagine old-style iterators
for which is_swappable returns false negatives.

:Proposed Resolution: See the resolution to 9.15.


9.18 Are is_readable, is_writable, and is_swappable useful? 
===========================================================

:Submitter: Dave Abrahams 
:Status: New 

I am concerned that there is little use for any of is_readable,
is_writable, or is_swappable, and that not only do they unduly
constrain iterator implementors but they add overhead to
iterator_facade and iterator_adaptor in the form of a template
parameter which would otherwise be unneeded. Since we can't
implement two of them accurately for old-style iterators, I am
having a hard time justifying their impact on the rest of the
proposal(s).

:Proposed Resolution: See the resolution to 9.15.

9.19 Non-Uniformity of the "lvalue_iterator Bit" 
================================================

:Submitter: Dave Abrahams 
:Status: New 

The proposed iterator_tag class template accepts an "access bits"
parameter which includes a bit to indicate the iterator's
lvalueness (whether its dereference operator returns a reference to
its value_type. The relevant part of N1550 says:

  The purpose of the lvalue_iterator part of the iterator_access
  enum is to communicate to iterator_tagwhether the reference type
  is an lvalue so that the appropriate old category can be chosen
  for the base class. The lvalue_iterator bit is not recorded in
  the iterator_tag::access data member.

The lvalue_iterator bit is not recorded because N1550 aims to
improve orthogonality of the iterator concepts, and a new-style
iterator's lvalueness is detectable by examining its reference
type. This inside/outside difference is awkward and confusing.

:Proposed Resolution: The iterator_tag class will be removed, so this is no longer an issue.
  See the resolution to 9.15.


9.20 Traversal Concepts and Tags 
================================

:Submitter: Dave Abrahams 
:Status: New 

Howard Hinnant pointed out some inconsistencies with the naming of
these tag types::

  incrementable_iterator_tag // ++r, r++ 
  single_pass_iterator_tag // adds a == b, a != b 
  forward_traversal_iterator_tag // adds multi-pass 
  bidirectional_traversal_iterator_tag // adds --r, r--
  random_access_traversal_iterator_tag // adds r+n,n+r,etc. 

Howard thought that it might be better if all tag names contained
the word "traversal".  It's not clear that would result in the best
possible names, though. For example, incrementable iterators can
only make a single pass over their input. What really distinguishes
single pass iterators from incrementable iterators is not that they
can make a single pass, but that they are equality
comparable. Forward traversal iterators really distinguish
themselves by introducing multi-pass capability. Without entering
a "Parkinson's Bicycle Shed" type of discussion, it might be worth
giving the names of these tags (and the associated concepts) some
extra attention.

:Proposed resolution: Change the names of the traversal tags to the
  following names::

    incrementable_traversal_tag
    single_pass_traversal_tag
    forward_traversal_tag
    bidirectional_traversal_tag
    random_access_traversal_tag


  In [lib.iterator.traversal]:


  Change:

    +--------------------------------+-------------------------------+--------------------+
    |``traversal_category<X>::type`` |Convertible to                 |                    |
    |                                |``incrementable_iterator_tag`` |                    |
    +--------------------------------+-------------------------------+--------------------+

  to:

    +--------------------------------+-------------------------------+--------------------+
    |``iterator_traversal<X>::type`` |Convertible to                 |                    |
    |                                |``incrementable_traversal_tag``|                    |
    +--------------------------------+-------------------------------+--------------------+

  Change:

    +--------------------------------+-----------------------------+---------------------------+
    |``traversal_category<X>::type`` |Convertible to               |                           |
    |                                |``single_pass_iterator_tag`` |                           |
    +--------------------------------+-----------------------------+---------------------------+

  to:

    +--------------------------------+-----------------------------+---------------------------+
    |``iterator_traversal<X>::type`` |Convertible to               |                           |
    |                                |``single_pass_traversal_tag``|                           |
    +--------------------------------+-----------------------------+---------------------------+

  Change:

    +---------------------------------------+-----------------------------------+---------------+
    |``traversal_category<X>::type``        |Convertible to                     |               |
    |                                       |``forward_traversal_iterator_tag`` |               |
    +---------------------------------------+-----------------------------------+---------------+

  to:

    +---------------------------------------+-----------------------------------+----------------------------+
    |``iterator_traversal<X>::type``        |Convertible to                     |                            |
    |                                       |``forward_traversal_tag``          |                            |
    +---------------------------------------+-----------------------------------+----------------------------+

  Change:

    +------------------------------------+---------------------------------------------+---------------------+
    |``traversal_category<X>::type``     |Convertible to                               |                     |
    |                                    |``bidirectional_traversal_iterator_tag``     |                     |
    +------------------------------------+---------------------------------------------+---------------------+

  to:

    +--------------------------------+-------------------------------+---------------------+
    |``iterator_traversal<X>::type`` |Convertible to                 |                     |
    |                                |``bidirectional_traversal_tag``|                     |
    +--------------------------------+-------------------------------+---------------------+

  Change:

    +-------------------------------------------+-------------------------------------------------+-------------------------+----------------------+
    |``traversal_category<X>::type``            |Convertible to                                   |                         |                      |
    |                                           |``random_access_traversal_iterator_tag``         |                         |                      |
    +-------------------------------------------+-------------------------------------------------+-------------------------+----------------------+

  to:

    +-------------------------------+---------------------------------+-------------------------+----------------------+
    |``iterator_traversal<X>::type``|Convertible to                   |                         |                      |
    |                               |``random_access_traversal_tag``  |                         |                      |
    +-------------------------------+---------------------------------+-------------------------+----------------------+


  In [lib.iterator.synopsis], change::  

    struct incrementable_iterator_tag { };
    struct single_pass_iterator_tag : incrementable_iterator_tag { };
    struct forward_traversal_tag : single_pass_iterator_tag { };

  to::

    struct incrementable_traversal_tag { };
    struct single_pass_traversal_tag : incrementable_traversal_tag { };
    struct forward_traversal_tag : single_pass_traversal_tag { };

  Remove::

    struct null_category_tag { };
    struct input_output_iterator_tag : input_iterator_tag, output_iterator_tag {};


9.21 iterator_facade Derived template argument underspecified 
=============================================================

:Submitter: Pete Becker 
:Status: New 

The first template argument to iterator_facade is named Derived,
and the proposal says: 

  The Derived template parameter must be a class derived from
  iterator_facade. 

First, iterator_facade is a template, so cannot be derived
from. Rather, the class must be derived from a specialization of
iterator_facade. More important, isn't Derived required to be the
class that is being defined? That is, if I understand it right, the
definition of D here this is not valid::

  class C : public iterator_facade<C, ... > { ... }; 
  class D : public iterator_facade<C, ...> { ... }; 

In the definition of D, the Derived argument to iterator_facade is
a class derived from a specialization of iterator_facade, so the
requirement is met. Shouldn't the requirement be more like "when
using iterator_facade to define an iterator class Iter, the class
Iter must be derived from a specialization of iterator_facade whose
first template argument is Iter." That's a bit awkward, but at the
moment I don't see a better way of phrasing it.

:Proposed resolution: 

  In [lib.iterator.facade]

  Remove:

    The ``Derived`` template parameter must be a class derived from
    ``iterator_facade``.
    
  Change:

    The following table describes the other requirements on the
    ``Derived`` parameter.  Depending on the resulting iterator's
    ``iterator_category``, a subset of the expressions listed in the table
    are required to be valid.  The operations in the first column must be
    accessible to member functions of class ``iterator_core_access``.
    
  to:

    The following table describes the typical valid expressions on
    ``iterator_facade``\ 's ``Derived`` parameter, depending on the
    iterator concept(s) it will model.  The operations in the first
    column must be made accessible to member functions of class
    ``iterator_core_access``.  In addition,
    ``static_cast<Derived*>(iterator_facade*)`` shall be well-formed.
    
  In [lib.iterator.adaptor]

  Change:

    The ``iterator_adaptor`` is a base class template derived from
    an instantiation of ``iterator_facade``.

  to:

    Each specialization of the ``iterator_adaptor`` class template
    is derived from a specialization of ``iterator_facade``.  

  Change:

    The ``Derived`` template parameter must be a derived class of
    ``iterator_adaptor``.

  To:

    ``static_cast<Derived*>(iterator_adaptor*)`` shall be well-formed.

[Note: The proposed resolution to Issue 9.37 contains related
changes]

9.22 return type of Iterator difference for iterator facade
===========================================================

:Submitter: Pete Becker 
:Status: New 

The proposal says::

  template <class Dr1, class V1, class AC1, class TC1, class R1, class D1, 
  class Dr2, class V2, class AC2, class TC2, class R2, class D2>
  typename enable_if_interoperable<Dr1, Dr2, bool>::type
  operator -(iterator_facade<Dr1, V1, AC1, TC1, R1, D1> const& lhs, 
  iterator_facade<Dr2, V2, AC2, TC2, R2, D2> const& rhs); 

Shouldn't the return type be one of the two iterator types? Which
one? The idea is that if one of the iterator types can be converted
to the other type, then the subtraction is okay. Seems like the
return type should then be the type that was converted to. Is that
right?

:Proposed resolution:

   See resolution to 9.34.

9.23 Iterator_facade: minor wording Issue
=========================================

:Submitter: Pete Becker 
:Status: New 

In the table that lists the required (sort of) member functions of
iterator types that are based on iterator_facade, the entry for
c.equal(y) says:

  true iff c and y refer to the same position. Implements c == y
  and c != y.  The second sentence is inside out. c.equal(y) does
  not implement either of these operations. It is used to implement
  them. Same thing in the description of c.distance_to(z).

:Proposed resolution: remove "implements" descriptions from
   table.  See resolution to 9.34


9.24 Use of undefined name in iterator_facade table 
===================================================

:Submitter: Pete Becker 
:Status: New 

Several of the descriptions use the name X without defining
it. This seems to be a carryover from the table immediately above
this section, but the text preceding that table says "In the table
below, X is the derived iterator type." Looks like the X::
qualifiers aren't really needed; X::reference can simply be
reference, since that's defined by the iterator_facade
specialization itself.

:Proposed resolution: 

  Remove references to X.

  In [lib.iterator.facade] operations ``operator->() const;``:

    Change:

      :Returns: If ``X::reference`` is a reference type, an object
        of type ``X::pointer`` equal to::

          &static_cast<Derived const*>(this)->dereference()

	Otherwise returns an object of unspecified type such that,
	given an object ``a`` of type ``X``, ``a->m`` is equivalent
	to ``(w = *a, w.m)`` for some temporary object ``w`` of type
	``X::value_type``.

        The type ``X::pointer`` is ``Value*`` if
        ``is_writable_iterator<X>::value`` is ``true``, and
        ``Value const*`` otherwise.

    to:

      :Returns: If ``reference`` is a reference type, an object
        of type ``pointer`` equal to::

          &static_cast<Derived const*>(this)->dereference()

        Otherwise returns an object of unspecified type such that,
        ``(*static_cast<Derived const*>(this))->m`` is equivalent
        to ``(w = **static_cast<Derived const*>(this), w.m)`` for
        some temporary object ``w`` of type ``value_type``.

    Further changes are covered by issue 9.26.

9.25 Iterator_facade: wrong return type 
=======================================

:Submitter: Pete Becker 
:Status: New 

Several of the member functions return a Derived object or a
Derived&. Their Effects clauses end with::

  return *this;

This should be ::

  return *static_cast<Derived*>(this);

:Proposed resolution: 

  In [lib.iterator.facade], in the effects clause
  of the following operations::

    Derived& operator++()
    Derived& operator--()
    Derived& operator+=(difference_type n)
    Derived& operator-=(difference_type n)

  Change:
    ``return *this``

  to:
    ``return *static_cast<Derived*>(this);``

9.26 Iterator_facade: unclear returns clause for operator[] 
===========================================================

:Submitter: Pete Becker 
:Status: New 

The returns clause for ``operator[](difference_type n)`` const
says: 

  Returns: an object convertible to X::reference and holding a copy
  p of a+n such that, for a constant object v of type
  X::value_type, X::reference(a[n] = v) is equivalent to p = v.
  This needs to define 'a', but assuming it's supposed to be
  ``*this`` (or maybe ``*(Derived*)this``), it still isn't clear
  what this says. Presumably, the idea is that you can index off of
  an iterator and assign to the result. But why the requirement
  that it hold a copy of a+n? Granted, that's probably how it's
  implemented, but it seems over-constrained. And the last phrase
  seems wrong. p is an iterator; there's no requirement that you
  can assign a value_type object to it. Should that be ``*p = v``?
  But why the cast in reference(a[n] = v)?

:Proposed resolution: 

  In section operator[]:
 
    Change:
  
      Writable iterators built with ``iterator_facade`` implement
      the semantics required by the preferred resolution to `issue
      299` and adopted by proposal `n1477`: the result of ``p[n]``
      is a proxy object containing a copy of ``p+n``, and ``p[n] =
      x`` is equivalent to ``*(p + n) = x``.  This approach will
      work properly for any random-access iterator regardless of
      the other details of its implementation.  A user who knows
      more about the implementation of her iterator is free to
      implement an ``operator[]`` which returns an lvalue in the
      derived iterator class; it will hide the one supplied by
      ``iterator_facade`` from clients of her iterator.
  
    to:
  
      Writable iterators built with ``iterator_facade`` implement
      the semantics required by the preferred resolution to `issue
      299` and adopted by proposal `n1550`: the result of ``p[n]``
      is an object convertible to the iterator's ``value_type``,
      and ``p[n] = x`` is equivalent to ``*(p + n) = x`` (Note:
      This result object may be implemented as a proxy containing a
      copy of ``p+n``).  This approach will work properly for any
      random-access iterator regardless of the other details of its
      implementation.  A user who knows more about the
      implementation of her iterator is free to implement an
      ``operator[]`` that returns an lvalue in the derived iterator
      class; it will hide the one supplied by ``iterator_facade``
      from clients of her iterator.
  
  In [lib.iterator.facade] operations:

    Change:
  
     :Returns: an object convertible to ``X::reference`` and
        holding a copy *p* of ``a+n`` such that, for a constant
        object ``v`` of type ``X::value_type``, ``X::reference(a[n]
        = v)`` is equivalent to ``p = v``.
  
    to:
  
     :Returns: an object convertible to ``value_type``. For
        constant objects ``v`` of type ``value_type``, and ``n`` of
        type ``difference_type``, ``(*this)[n] = v`` is equivalent
        to ``*(*this + n) = v``, and ``static_cast<value_type
        const&>((*this)[n])`` is equivalent to
        ``static_cast<value_type const&>(*(*this + n))``
     

9.27 Iterator_facade: redundant clause 
======================================

:Submitter: Pete Becker 
:Status: New 

``operator-`` has both an effects clause and a returns
clause. Looks like the returns clause should be removed.

:Proposed resolution:

  Remove the returns clause.

  In [lib.iterator.facade] operations:
        
  Remove:
    :Returns: ``static_cast<Derived const*>(this)->advance(-n);`` 




9.28 indirect_iterator: incorrect specification of default constructor 
======================================================================

:Submitter: Pete Becker 
:Status: New 

The default constructor returns "An instance of indirect_iterator
with a default constructed base object", but the constructor that
takes an Iterator object returns "An instance of indirect_iterator
with the iterator_adaptor subobject copy constructed from x." The
latter is the correct form, since it does not reach inside the base
class for its semantics. So the default constructor shoudl return
"An instance of indirect_iterator with a default-constructed
iterator_adaptor subobject."

:Proposed resolution: 
  Change:
     :Returns: An instance of ``indirect_iterator`` with
        a default constructed base object.

  to:
     :Returns: An instance of ``indirect_iterator`` with
       a default-constructed ``m_iterator``.

:Rationale: Inheritance from iterator_adaptor has been removed, so we instead
  give the semantics in terms of the (exposition only) member
  ``m_iterator``.


9.29 indirect_iterator: unclear specification of template constructor 
=====================================================================

:Submitter: Pete Becker 
:Status: New 

The templated constructor that takes an indirect_iterator with a
different set of template arguments says that it returns "An
instance of indirect_iterator that is a copy of [the argument]".
But the type of the argument is different from the type of the
object being constructed, and there is no description of what
a "copy" means. The Iterator template parameter for the argument
must be convertible to the Iterator template parameter for the type
being constructed, which suggests that the argument's contained
Iterator object should be converted to the target type's Iterator
type.  Is that what's meant here?
(Pete later writes: In fact, this problem is present in all of the
specialized adaptors that have a constructor like this: the
constructor returns "a copy" of the argument without saying what a
copy is.)

:Proposed resolution: 

  Change:
    :Returns: An instance of ``indirect_iterator`` that is a copy of ``y``.

  to:
    :Returns: An instance of ``indirect_iterator`` whose 
      ``m_iterator`` subobject is constructed from ``y.base()``.


:Rationale: Inheritance from iterator_adaptor has been removed, so we
  instead give the semantics in terms of the member ``m_iterator``.


9.30 transform_iterator argument irregularity 
=============================================

:Submitter: Pete Becker 
:Status: New 

The specialized adaptors that take both a Value and a Reference
template argument all take them in that order, i.e. Value precedes
Reference in the template argument list, with the exception of
transform_iterator, where Reference precedes Value. This seems like
a possible source of confusion. Is there a reason why this order is
preferable?

:Proposed resolution: NAD

:Rationale: defaults for Value depend on Reference.  A sensible
   Value can almost always be computed from Reference.  The first
   parameter is UnaryFunction, so the argument order is already
   different from the other adapters.


9.31 function_output_iterator overconstrained 
=============================================

:Submitter: Pete Becker 
:Status: New 

function_output_iterator requirements says: "The UnaryFunction must
be Assignable, Copy Constructible, and the expression f(x) must be
valid, where f is an object of type UnaryFunction and x is an
object of a type accepted by f."

Everything starting with "and," somewhat reworded, is actually a
constraint on output_proxy::operator=. All that's needed to create
a function_output_iterator object is that the UnaryFunction type be
Assignable and CopyConstructible. That's also sufficient to
dereference and to increment such an object. It's only when you try
to assign through a dereferenced iterator that f(x) has to work,
and then only for the particular function object that the iterator
holds and for the particular value that is being assigned.


Addition from Jeremy:
  The constructor for ``function_output_iterator`` is also
  slightly overconstrained because it requires 
  the ``UnaryFunction`` to have a default constructor
  even when the default constructor of ``function_output_iterator``
  is not used.


:Proposed resolution: 

  Change:
    ``output_proxy operator*();``

  to:
    ``/* see below */ operator*();``

  After ``function_output_iterator& operator++(int);`` add::

    private:
      UnaryFunction m_f;     // exposition only

  Change:
    The ``UnaryFunction`` must be Assignable, Copy Constructible,
    and the expression ``f(x)`` must be valid, where ``f`` is an
    object of type ``UnaryFunction`` and ``x`` is an object of a
    type accepted by ``f``.  The resulting
    ``function_output_iterator`` is a model of the Writable and
    Incrementable Iterator concepts.

  to:
    ``UnaryFunction`` must be Assignable and Copy Constructible.  

  After the requirements section, add:

.. topic:: ``function_output_iterator`` models

    ``function_output_iterator`` is a model of the Writable and
    Incrementable Iterator concepts.

  Change:
    :Returns: An instance of ``function_output_iterator`` with
      ``f`` stored as a data member.

  to:
    :Effects: Constructs an instance of ``function_output_iterator`` 
      with ``m_f`` constructed from ``f``.

  Change:
    ``output_proxy operator*();``

    :Returns: An instance of ``output_proxy`` constructed with
      a copy of the unary function ``f``.

  to:
    ``operator*();``

    :Returns: An object ``r`` of unspecified type such that ``r = t``
      is equivalent to ``m_f(t)`` for all ``t``.

  
  Remove:
    ``function_output_iterator::output_proxy`` operations

    ``output_proxy(UnaryFunction& f);``

    :Returns: An instance of ``output_proxy`` with ``f`` stored as
        a data member.


    ``template <class T> output_proxy& operator=(const T& value);``

    :Effects: 
      ::

          m_f(value); 
          return *this; 


  Change::

    explicit function_output_iterator(const UnaryFunction& f = UnaryFunction());

  to::

    explicit function_output_iterator();

    explicit function_output_iterator(const UnaryFunction& f);



9.32 Should output_proxy really be a named type? 
================================================

:Submitter: Pete Becker 
:Status: New 

This means someone can store an output_proxy object for later use,
whatever that means. It also constrains output_proxy to hold a copy
of the function object, rather than a pointer to the iterator
object. Is all this mechanism really necessary?

:Proposed resolution: See issue 9.31.



9.33 istreambuf_iterator isn't a Readable Iterator 
==================================================

:Submitter: Pete Becker 
:Status: New 

c++std-lib-12333: 

  N1550 requires that for a Readable Iterator a of type X, ``*a``
  returns an object of type
  ``iterator_traits<X>::reference``. ``istreambuf_iterator::operator*``
  returns ``charT``, but ``istreambuf_iterator::reference`` is
  ``charT&``. So am I overlooking something, or is
  ``istreambuf_iterator`` not Readable.

:Proposed resolution: Remove all constraints on
  ``iterator_traits<X>::reference`` in Readable Iterator and Lvalue
  Iterator. Change Lvalue Iterator to refer to ``T&`` instead of
  ``iterator_traits<X>::reference``.

  Change:
    A class or built-in type ``X`` models the *Readable Iterator*
    concept for the value type ``T`` if the following expressions
    are valid and respect the stated semantics. ``U`` is the type
    of any specified member of type ``T``.

  to:
    A class or built-in type ``X`` models the *Readable Iterator*
    concept for value type ``T`` if, in addition to ``X`` being
    Assignable and Copy Constructible, the following expressions
    are valid and respect the stated semantics. ``U`` is the type
    of any specified member of type ``T``.

  From the Input Iterator Requirements table, remove:

    +-----------------------------------+-----------------------------------+-------------------------+
    |``iterator_traits<X>::reference``  |Convertible to                     |                         |
    |                                   |``iterator_traits<X>::value_type`` |                         |
    +-----------------------------------+-----------------------------------+-------------------------+

  Change:

    +-----------------------------------+-----------------------------------+-------------------------+
    |``*a``                             |``iterator_traits<X>::reference``  |pre: ``a`` is            |
    |                                   |                                   |dereferenceable. If ``a  |
    |                                   |                                   |== b`` then ``*a`` is    |
    |                                   |                                   |equivalent to ``*b``     |
    +-----------------------------------+-----------------------------------+-------------------------+

  to:

    +-----------------------------------+------------------------+----------------------------------------------------------------+
    |``*a``                             | Convertible to ``T``   |pre: ``a`` is dereferenceable. If ``a == b`` then ``*a``        |
    |                                   |                        | is equivalent to ``*b``.                                       |
    +-----------------------------------+------------------------+----------------------------------------------------------------+

  Change:
    The *Lvalue Iterator* concept adds the requirement that the
    ``reference`` type be a reference to the value type of the
    iterator.

  to:
    The *Lvalue Iterator* concept adds the requirement that the
    return type of ``operator*`` type be a reference to the value
    type of the iterator.

  Change:

    +---------------------------------------------------------------------------------+
    | Lvalue Iterator Requirements                                                    |
    +---------------------------------+-----------+-----------------------------------+
    |Expression                       |Return Type|Assertion                          |
    +=================================+===========+===================================+
    |``iterator_traits<X>::reference``|``T&``     |``T`` is *cv*                      |
    |                                 |           |``iterator_traits<X>::value_type`` |
    |                                 |           |where *cv* is an optional          |
    |                                 |           |cv-qualification                   |
    +---------------------------------+-----------+-----------------------------------+

  to:

    +-------------------------------------------------------------+
    | Lvalue Iterator Requirements                                |
    +-------------+-----------+-----------------------------------+
    |Expression   |Return Type|Note/Assertion                     |
    +=============+===========+===================================+
    |``*a``       | ``T&``    |``T`` is *cv*                      |
    |             |           |``iterator_traits<X>::value_type`` |
    |             |           |where *cv* is an optional          |
    |             |           |cv-qualification.                  |
    |             |           |pre: ``a`` is                      |
    |             |           |dereferenceable. If ``a            |
    |             |           |== b`` then ``*a`` is              |
    |             |           |equivalent to ``*b``.              |
    +-------------+-----------+-----------------------------------+


  At the end of the section reverse_iterator models, add:
  The type ``iterator_traits<Iterator>::reference`` must be the type of
  ``*i``, where ``i`` is an object of type ``Iterator``.


:Rationale: Ideally there should be requirements on the reference
  type, however, since Readable Iterator is suppose to correspond
  to the current standard iterator requirements (which do not place
  requirements on the reference type) we will leave them off for
  now.  There is a DR in process with respect to the reference type
  in the stadard iterator requirements. Once that is resolved we
  will revisit this issue for Readable Iterator and Lvalue
  Iterator.

  We added Assignable to the requirements for Readable
  Iterator. This is needed to have Readable Iterator coincide with
  the capabilities of Input Iterator.


9.34 iterator_facade free functions unspecified
===============================================

:Submitter: Pete Becker 
:Status: New 

c++std-lib-12562:

  The template functions ``operator==``, ``operator!=``,
  ``operator<``, ``operator<=``, ``operator>``, ``operator>=``, and
  ``operator-`` that take two arguments that are specializations of
  iterator_facade have no specification. The template function
  operator+ that takes an argument that is a specialization of
  iterator_facade and an argument of type difference_type has no
  specification.

:Proposed resolution: 
  Add the missing specifications.

  ::

    template <class Dr, class V, class TC, class R, class D>
    Derived operator+ (iterator_facade<Dr,V,TC,R,D> const&,
                       typename Derived::difference_type n);

    template <class Dr, class V, class TC, class R, class D>
    Derived operator+ (typename Derived::difference_type n,
                       iterator_facade<Dr,V,TC,R,D> const&);

  :Effects:
    ::

      Derived tmp(static_cast<Derived const*>(this));
      return tmp += n;


  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator ==(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``lhs.equal(rhs)``.  Otherwise, ``rhs.equal(lhs)``.

  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator !=(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``!lhs.equal(rhs)``.  Otherwise, ``!rhs.equal(lhs)``.

  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator <(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
               iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``lhs.distance_to(rhs) < 0``. Otherwise, ``rhs.distance_to(lhs) >
    0``.

  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator <=(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``lhs.distance_to(rhs) <= 0``. Otherwise, ``rhs.distance_to(lhs)
    >= 0``.

  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator >(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
               iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``lhs.distance_to(rhs) > 0``. Otherwise,
    ``rhs.distance_to(lhs) < 0``.


  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,bool>::type
    operator >=(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
                iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``lhs.distance_to(rhs) >= 0``. Otherwise,
    ``rhs.distance_to(lhs) <= 0``.

  ::

    template <class Dr1, class V1, class TC1, class R1, class D1,
              class Dr2, class V2, class TC2, class R2, class D2>
    typename enable_if_interoperable<Dr1,Dr2,difference>::type
    operator -(iterator_facade<Dr1,V1,TC1,R1,D1> const& lhs,
               iterator_facade<Dr2,V2,TC2,R2,D2> const& rhs);

  :Return Type: if ``is_convertible<Dr2,Dr1>::value``, then
    ``difference`` shall be
    ``iterator_traits<Dr1>::difference_type``.  Otherwise,
    ``difference`` shall be
    ``iterator_traits<Dr2>::difference_type``.

  :Returns: if ``is_convertible<Dr2,Dr1>::value``, then
    ``-lhs.distance_to(rhs)``. Otherwise,
    ``rhs.distance_to(lhs)``.



9.35 iterator_facade: too many equals? 
======================================

:Submitter: Pete Becker 
:Status: New 

c++std-lib-12563:

  The table listing the functions required for types derived from
  iterator_facade has two functions named equal and two named
  distance_to::

    c.equal(b)
    c.equal(y)
    c.distance_to(b)
    c.distance_to(z)

  where b and c are const objects of the derived type, y and z are
  constant objects of certain iterator types that are interoperable
  with the derived type.  Seems like the 'b' versions are
  redundant: in both cases, the other version will take a 'b'. In
  fact, iterator_adaptor is specified to use iterator_facade, but
  does not provide the 'b' versions of these functions.

  Are the 'b' versions needed? 

:Proposed resolution:  Remove the 'b' versions.

  In ``iterator_facade`` requirements, remove:

    +--------------------+-------------------+-------------------------------------+---------------------------+
    |``c.equal(b)``      |convertible to bool|true iff ``b`` and ``c`` are         |Single Pass Iterator       |
    |                    |                   |equivalent.                          |                           |
    +--------------------+-------------------+-------------------------------------+---------------------------+

  and remove:

    +--------------------+-------------------+-------------------------------------+---------------------------+
    |``c.distance_to(b)``|convertible to     |equivalent to ``distance(c, b)``     |Random Access Traversal    |
    |                    |X::difference_type |                                     |Iterator                   |
    +--------------------+-------------------+-------------------------------------+---------------------------+


9.36 iterator_facade function requirements 
==========================================

:Submitter: Pete Becker 
:Status: New 

c++std-lib-12636:

  The table that lists required functions for the derived type X
  passed to iterator_facade lists, among others:

  for a single pass iterator::

    c.equal(b)
    c.equal(y)

  where b and c are const X objects, and y is a const object of a
  single pass iterator that is interoperable with X. Since X is
  interoperable with itself, c.equal(b) is redundant. There is a
  difference in their descriptions, but its meaning isn't
  clear. The first is "true iff b and c are equivalent", and the
  second is "true iff c and y refer to the same position." Is there
  a difference between the undefined term "equivalent" and "refer
  to the same position"?

  Similarly, for a random access traversal iterator::

    c.distance_to(b)
    c.distance_to(z)

  where z is a constant object of a random access traversal
  iterator that is interoperable with X.  Again, X is interoperable
  with itself, so c.distance_to(b) is redundant.  Also, the
  specification for c.distance_to(z) isn't valid. It's written
  as "equivalent to distance(c, z)". The template function distance
  takes two arguments of the same type, so distance(c, z) isn't
  valid if c and z are different types. Should it be
  distance(c, (X)z)?

:Proposed resolution:   Removed the 'b' versions (see 9.35) and added the cast.

  Change:

    +--------------------+-------------------+-------------------------------------+---------------------------+
    |``c.distance_to(z)``|convertible to     |equivalent to ``distance(c, z)``.    |Random Access Traversal    |
    |                    |X::difference_type |Implements ``c - z``, ``c < z``, ``c |Iterator                   |
    |                    |                   |<= z``, ``c > z``, and ``c >= c``.   |                           |
    +--------------------+-------------------+-------------------------------------+---------------------------+

  to:

    +--------------------+----------------------+-------------------------+---------------------------+
    |``c.distance_to(z)``|convertible to        |equivalent to            |Random Access Traversal    |
    |                    |``F::difference_type``|``distance(c, X(z))``.   |Iterator                   |
    +--------------------+----------------------+-------------------------+---------------------------+



====================================
 More Issues (not from Matt's list)
====================================



9.37x Inheritance in iterator_adaptor and other adaptors is an overspecification
================================================================================

:Submitter: Pete Becker
:Status: New 

c++std-lib-12696:
The paper requires that iterator_adaptor be derived from an
appropriate instance of iterator_facade, and that most of the specific
forms of adaptors be derived from appropriate instances of
iterator_adaptor. That seems like overspecification, and we ought to
look at specifying these things in terms of what the various templates
provide rather than how they're implemented.

:Proposed resolution:

  Remove the specfication of inheritance, and add explicit
  specification of all the functionality that was inherited from the
  specialized iterators. 

  In iterator_adaptor, inheritance is retained, sorry NAD.  Also,
  the Interoperable Iterators concept is added to the new iterator
  concepts, and this concept is used in the specification of the
  iterator adaptors.


  In n1550, after [lib.random.access.traversal.iterators], add:

    Interoperable Iterators [lib.interoperable.iterators]

    A class or built-in type ``X`` that models Single Pass Iterator
    is *interoperable with* a class or built-in type ``Y`` that
    also models Single Pass Iterator if the following expressions
    are valid and respect the stated semantics. In the tables
    below, ``x`` is an object of type ``X``, ``y`` is an object of
    type ``Y``, ``Distance`` is
    ``iterator_traits<Y>::difference_type``, and ``n`` represents a
    constant object of type ``Distance``.

    +-----------+-----------------------+---------------------------------------------------+
    |Expression |Return Type            |Assertion/Precondition/Postcondition               |
    +===========+=======================+===================================================+
    |``y = x``  |``Y``                  |post: ``y == x``                                   |
    +-----------+-----------------------+---------------------------------------------------+
    |``Y(x)``   |``Y``                  |post: ``Y(x) == x``                                |
    +-----------+-----------------------+---------------------------------------------------+
    |``x == y`` |convertible to ``bool``|``==`` is an equivalence relation over its domain. |
    +-----------+-----------------------+---------------------------------------------------+
    |``y == x`` |convertible to ``bool``|``==`` is an equivalence relation over its domain. |
    +-----------+-----------------------+---------------------------------------------------+
    |``x != y`` |convertible to ``bool``|``bool(a==b) != bool(a!=b)`` over its domain.      |
    +-----------+-----------------------+---------------------------------------------------+
    |``y != x`` |convertible to ``bool``|``bool(a==b) != bool(a!=b)`` over its domain.      |
    +-----------+-----------------------+---------------------------------------------------+

    If ``X`` and ``Y`` both model Random Access Traversal Iterator then
    the following additional requirements must be met.

    +-----------+-----------------------+---------------------+--------------------------------------+
    |Expression |Return Type            |Operational Semantics|Assertion/ Precondition               |
    +===========+=======================+=====================+======================================+
    |``x < y``  |convertible to ``bool``|``y - x > 0``        |``<`` is a total ordering relation    |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``y < x``  |convertible to ``bool``|``x - y > 0``        |``<`` is a total ordering relation    |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``x > y``  |convertible to ``bool``|``y < x``            |``>`` is a total ordering relation    |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``y > x``  |convertible to ``bool``|``x < y``            |``>`` is a total ordering relation    |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``x >= y`` |convertible to ``bool``|``!(x < y)``         |                                      |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``y >= x`` |convertible to ``bool``|``!(y < x)``         |                                      |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``x <= y`` |convertible to ``bool``|``!(x > y)``         |                                      |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``y <= x`` |convertible to ``bool``|``!(y > x)``         |                                      |
    +-----------+-----------------------+---------------------+--------------------------------------+
    |``y - x``  |``Distance``           |``distance(Y(x),y)`` |pre: there exists a value ``n`` of    |
    |           |                       |                     |``Distance`` such that ``x + n == y``.|
    |           |                       |                     |``y == x + (y - x)``.                 |
    +-----------+-----------------------+---------------------+--------------------------------------+ 
    |``x - y``  |``Distance``           |``distance(y,Y(x))`` |pre: there exists a value ``n`` of    |
    |           |                       |                     |``Distance`` such that ``y + n == x``.|
    |           |                       |                     |``x == y + (x - y)``.                 |
    +-----------+-----------------------+---------------------+--------------------------------------+



  In N1530:

    In [lib.iterator.adaptor]

    Change::

       class iterator_adaptor 
         : public iterator_facade<Derived, /* see details ...*/>

    To::

       class iterator_adaptor 
         : public iterator_facade<Derived, *V'*, *C'*, *R'*, *D'*> // see details
      

    Change the text from:
       The ``Base`` type must implement the expressions involving
       ``m_iterator`` in the specifications...

    until the end of the **iterator_adaptor requirements** section, to:
       The ``Base`` argument shall be Assignable and Copy Constructible.


    Add:

.. topic:: ``iterator_adaptor`` base class parameters

      The *V'*, *C'*, *R'*, and *D'* parameters of the ``iterator_facade``
      used as a base class in the summary of ``iterator_adaptor``
      above are defined as follows:

      .. parsed-literal::

         *V'* = if (Value is use_default)
                   return iterator_traits<Base>::value_type
               else
                   return Value

         *C'* = if (CategoryOrTraversal is use_default)
                   return iterator_traversal<Base>::type
               else
                   return CategoryOrTraversal

         *R'* = if (Reference is use_default)
                   if (Value is use_default)
                       return iterator_traits<Base>::reference
                   else
                       return Value&
               else
                   return Reference

         *D'* = if (Difference is use_default)
                   return iterator_traits<Base>::difference_type
               else
                   return Difference


    In [lib.iterator.special.adaptors]

    Change::

      class indirect_iterator
        : public iterator_adaptor</* see discussion */>
      {
          friend class iterator_core_access;

    to::

      class indirect_iterator
      {
       public:
          typedef /* see below */ value_type;
          typedef /* see below */ reference;
          typedef /* see below */ pointer;
          typedef /* see below */ difference_type;
          typedef /* see below */ iterator_category;

    Change::

      private: // as-if specification
          typename indirect_iterator::reference dereference() const
          {
              return **this->base();
          }

    to::

          Iterator const& base() const;
          reference operator*() const;
          indirect_iterator& operator++();
          indirect_iterator& operator--();
      private:
         Iterator m_iterator; // exposition


    After the synopsis add:

      The member types of ``indirect_iterator`` are defined
      according to the following pseudo-code, where ``V`` is
      ``iterator_traits<Iterator>::value_type``

      .. parsed-literal::

        if (Value is use_default) then
            typedef remove_const<pointee<V>::type>::type value_type;
        else
            typedef remove_const<Value>::type value_type;

        if (Reference is use_default) then
            if (Value is use_default) then
                typedef indirect_reference<V>::type reference;
            else
                typedef Value& reference;
        else
            typedef Reference reference;

        if (Value is use_default) then 
            typedef pointee<V>::type\* pointer;
        else 
            typedef Value\* pointer;

        if (Difference is use_default)
            typedef iterator_traits<Iterator>::difference_type difference_type;
        else
            typedef Difference difference_type;

        if (CategoryOrTraversal is use_default)
            typedef |iterator-category|\ (
                iterator_traversal<Iterator>::type,``reference``,``value_type``
            ) iterator_category;
        else
            typedef |iterator-category|\ (
                CategoryOrTraversal,``reference``,``value_type``
            ) iterator_category;


    [Note: See resolution to 9.44y for a description of ``pointee`` and
    ``indirect_reference``]

    After the requirements section, add:

.. topic:: ``indirect_iterator`` models


      In addition to the concepts indicated by ``iterator_category``
      and by ``iterator_traversal<indirect_iterator>::type``, a
      specialization of ``indirect_iterator`` models the following
      concepts, Where ``v`` is an object of
      ``iterator_traits<Iterator>::value_type``:

        * Readable Iterator if ``reference(*v)`` is convertible to
          ``value_type``.

        * Writable Iterator if ``reference(*v) = t`` is a valid
          expression (where ``t`` is an object of type
          ``indirect_iterator::value_type``)

        * Lvalue Iterator if ``reference`` is a reference type.

      ``indirect_iterator<X,V1,C1,R1,D1>`` is interoperable with
      ``indirect_iterator<Y,V2,C2,R2,D2>`` if and only if ``X`` is
      interoperable with ``Y``.


    Before ``indirect_iterator();`` add:

      In addition to the operations required by the concepts described
      above, specializations of ``indirect_iterator`` provide the
      following operations.

    Change:
      :Returns: An instance of ``indirect_iterator`` with
        the ``iterator_adaptor`` subobject copy constructed from ``x``.

    to:
      :Returns: An instance of ``indirect_iterator`` with
        ``m_iterator`` copy constructed from ``x``.


    At the end of the indirect_iterator operations add:

      ``Iterator const& base() const;``

      :Returns: ``m_iterator``


      ``reference operator*() const;``

      :Returns:  ``**m_iterator``


      ``indirect_iterator& operator++();``

      :Effects: ``++m_iterator``
      :Returns: ``*this``


      ``indirect_iterator& operator--();``

      :Effects: ``--m_iterator``
      :Returns: ``*this``



    Change::

      template <class Iterator>
      class reverse_iterator :
        public iterator_adaptor< reverse_iterator<Iterator>, Iterator >
      {
        friend class iterator_core_access;

    to::

      template <class Iterator>
      class reverse_iterator
      {
      public:
        typedef iterator_traits<Iterator>::value_type value_type;
        typedef iterator_traits<Iterator>::reference reference;
        typedef iterator_traits<Iterator>::pointer pointer;
        typedef iterator_traits<Iterator>::difference_type difference_type;
        typedef /* see below */ iterator_category;

    Change::

      private: // as-if specification
        typename reverse_iterator::reference dereference() const { return *prior(this->base()); }

        void increment() { --this->base_reference(); }
        void decrement() { ++this->base_reference(); }

        void advance(typename reverse_iterator::difference_type n)
        {
            this->base_reference() += -n;
        }

        template <class OtherIterator>
        typename reverse_iterator::difference_type
        distance_to(reverse_iterator<OtherIterator> const& y) const
        {
            return this->base_reference() - y.base();
        }

    to::

        Iterator const& base() const;
        reference operator*() const;
        reverse_iterator& operator++();
        reverse_iterator& operator--();
      private:
        Iterator m_iterator; // exposition

    After the synopsis for ``reverse_iterator``, add:
      If ``Iterator`` models Random Access Traversal Iterator and Readable
      Lvalue Iterator, then ``iterator_category`` is convertible to
      ``random_access_iterator_tag``. Otherwise, if
      ``Iterator`` models Bidirectional Traversal Iterator and Readable
      Lvalue Iterator, then ``iterator_category`` is convertible to
      ``bidirectional_iterator_tag``. Otherwise, ``iterator_category`` is
      convertible to ``input_iterator_tag``.


    Change:
      **reverse_iterator requirements**

      The base ``Iterator`` must be a model of Bidirectional Traversal
      Iterator. The resulting ``reverse_iterator`` will be a model of the
      most refined standard traversal and access concepts that are modeled
      by ``Iterator``.


    to:
      **reverse_iterator requirements**

      ``Iterator`` must be a model of Bidirectional Traversal Iterator.


.. topic:: ``reverse_iterator`` models

      A specialization of ``reverse_iterator`` models the same iterator
      traversal and iterator access concepts modeled by its ``Iterator``
      argument.  In addition, it may model old iterator concepts
      specified in the following table:

      +---------------------------------------+-----------------------------------+
      | If ``I`` models                       |then ``reverse_iterator<I>`` models|
      +=======================================+===================================+
      | Readable Lvalue Iterator,             | Bidirectional Iterator            |
      | Bidirectional Traversal Iterator      |                                   |
      +---------------------------------------+-----------------------------------+
      | Writable Lvalue Iterator,             | Mutable Bidirectional Iterator    |
      | Bidirectional Traversal Iterator      |                                   |
      +---------------------------------------+-----------------------------------+
      | Readable Lvalue Iterator,             | Random Access Iterator            |
      | Random Access Traversal Iterator      |                                   |
      +---------------------------------------+-----------------------------------+
      | Writable Lvalue Iterator,             | Mutable Random Access Iterator    |
      | Random Access Traversal Iterator      |                                   |
      +---------------------------------------+-----------------------------------+


      ``reverse_iterator<X>`` is interoperable with
      ``reverse_iterator<Y>`` if and only if ``X`` is interoperable with
      ``Y``.

    Change:
      :Returns: An instance of ``reverse_iterator`` with a
        default constructed base object.

    to:
      :Effects: Constructs an instance of ``reverse_iterator`` with ``m_iterator`` 
        default constructed.

    Change:
      :Effects: Constructs an instance of ``reverse_iterator`` with a
        base object copy constructed from ``x``.

    to:
      :Effects: Constructs an instance of ``reverse_iterator`` with a
        ``m_iterator`` constructed from ``x``.


    Change:
      :Returns: An instance of ``reverse_iterator`` that is a copy of ``r``.

    to:
      :Effects: Constructs instance of ``reverse_iterator`` whose 
        ``m_iterator`` subobject is constructed from ``y.base()``.    



    At the end of the operations for ``reverse_iterator``, add:
      ``Iterator const& base() const;``

      :Returns: ``m_iterator``


      ``reference operator*() const;``

      :Effects: 

      ::

          Iterator tmp = m_iterator;
          return *--tmp;


      ``reverse_iterator& operator++();``

      :Effects: ``--m_iterator``
      :Returns: ``*this``


      ``reverse_iterator& operator--();``

      :Effects: ``++m_iterator``
      :Returns: ``*this``



    Change::

      class transform_iterator
        : public iterator_adaptor</* see discussion */>
      {
        friend class iterator_core_access;

    to::

      class transform_iterator
      {
      public:
        typedef /* see below */ value_type;
        typedef /* see below */ reference;
        typedef /* see below */ pointer;
        typedef iterator_traits<Iterator>::difference_type difference_type;
        typedef /* see below */ iterator_category;


    After ``UnaryFunction functor() const;`` add::

       Iterator const& base() const;
       reference operator*() const;
       transform_iterator& operator++();
       transform_iterator& operator--();

    Change::

       private:
         typename transform_iterator::value_type dereference() const;
         UnaryFunction m_f;
       };

    to::

       private:
         Iterator m_iterator; // exposition only
         UnaryFunction m_f;   // exposition only
       };


    After the synopsis, add:
      If ``Iterator`` models Readable Lvalue Iterator and if ``Iterator``
      models Random Access Traversal Iterator, then ``iterator_category`` is
      convertible to ``random_access_iterator_tag``. Otherwise, if
      ``Iterator`` models Bidirectional Traversal Iterator, then
      ``iterator_category`` is convertible to
      ``bidirectional_iterator_tag``.  Otherwise ``iterator_category`` is
      convertible to ``forward_iterator_tag``. If ``Iterator`` does not
      model Readable Lvalue Iterator then ``iterator_category`` is
      convertible to ``input_iterator_tag``.


    In the requirements section, change:
      The type ``Iterator`` must at least model Readable Iterator.  The
      resulting ``transform_iterator`` models the most refined of the
      following that is also modeled by ``Iterator``.

        * Writable Lvalue Iterator if
          ``result_of<UnaryFunction(iterator_traits<Iterator>::reference)>::type``
          is a non-const reference.

        * Readable Lvalue Iterator if
          ``result_of<UnaryFunction(iterator_traits<Iterator>::reference)>::type``
          is a const reference.

        * Readable Iterator otherwise.


      The ``transform_iterator`` models the most refined standard traversal
      concept that is modeled by ``Iterator``.

      The ``reference`` type of ``transform_iterator`` is
      ``result_of<UnaryFunction(iterator_traits<Iterator>::reference)>::type``.
      The ``value_type`` is ``remove_cv<remove_reference<reference> >::type``.

    to:
      The argument ``Iterator`` shall model Readable Iterator.


    After the requirements section, add:

.. topic:: ``transform_iterator`` models

      The resulting ``transform_iterator`` models the most refined of the
      following options that is also modeled by ``Iterator``.

        * Writable Lvalue Iterator if
          ``transform_iterator::reference`` is a non-const
          reference.

        * Readable Lvalue Iterator if
          ``transform_iterator::reference`` is a const reference.

        * Readable Iterator otherwise.

      The ``transform_iterator`` models the most refined standard traversal
      concept that is modeled by the ``Iterator`` argument.

      If ``transform_iterator`` is a model of Readable Lvalue Iterator then
      it models the following original iterator concepts depending on what
      the ``Iterator`` argument models.

      +-----------------------------------+-----------------------------------+
      | If ``Iterator`` models            |then ``transform_iterator`` models |
      +===================================+===================================+
      | Single Pass Iterator              | Input Iterator                    |
      +-----------------------------------+-----------------------------------+
      | Forward Traversal Iterator        | Forward Iterator                  |
      +-----------------------------------+-----------------------------------+
      | Bidirectional Traversal Iterator  | Bidirectional Iterator            |
      +-----------------------------------+-----------------------------------+
      | Random Access Traversal Iterator  | Random Access Iterator            |
      +-----------------------------------+-----------------------------------+

      If ``transform_iterator`` models Writable Lvalue Iterator then it is a
      mutable iterator (as defined in the old iterator requirements).

      ``transform_iterator<F1, X, R1, V1>`` is interoperable with
      ``transform_iterator<F2, Y, R2, V2>`` if and only if ``X`` is
      interoperable with ``Y``.


    Remove the private operations section heading and remove::

      ``typename transform_iterator::value_type dereference() const;``

      :Returns: ``m_f(transform_iterator::dereference());``

    After the entry for ``functor()``, add::

      ``Iterator const& base() const;``

      :Returns: ``m_iterator``


      ``reference operator*() const;``

      :Returns: ``m_f(*m_iterator)``


      ``transform_iterator& operator++();``

      :Effects: ``++m_iterator``
      :Returns: ``*this``


      ``transform_iterator& operator--();``

      :Effects: ``--m_iterator``
      :Returns: ``*this``


    Change::

      template <class Predicate, class Iterator>
      class filter_iterator
         : public iterator_adaptor<
               filter_iterator<Predicate, Iterator>, Iterator
             , use_default
             , /* see details */
           >
      {
       public:

    to::

      template <class Predicate, class Iterator>
      class filter_iterator
      {
       public:
         typedef iterator_traits<Iterator>::value_type value_type;
         typedef iterator_traits<Iterator>::reference reference;
         typedef iterator_traits<Iterator>::pointer pointer;
         typedef iterator_traits<Iterator>::difference_type difference_type;
         typedef /* see below */ iterator_category;

    Change::

      private: // as-if specification
         void increment()
         {
             ++(this->base_reference());
             satisfy_predicate();
         }

         void satisfy_predicate()
         {
             while (this->base() != this->m_end && !this->m_predicate(*this->base()))
                 ++(this->base_reference());
         }

         Predicate m_predicate;
         Iterator m_end;

    to::

         Iterator const& base() const;
         reference operator*() const;
         filter_iterator& operator++();
     private:
         Predicate m_pred; // exposition only
         Iterator m_iter;  // exposition only
         Iterator m_end;   // exposition only



    Change:
      The base ``Iterator`` parameter must be a model of Readable
      Iterator and Single Pass Iterator. The resulting
      ``filter_iterator`` will be a model of Forward Traversal Iterator
      if ``Iterator`` is, otherwise the ``filter_iterator`` will be a
      model of Single Pass Iterator. The access category of the
      ``filter_iterator`` will be the same as the access category of
      ``Iterator``.

    to:
      The ``Iterator`` argument shall meet the requirements of Readable
      Iterator and Single Pass Iterator or it shall meet the requirements of
      Input Iterator.

    After the requirements section, add:

.. topic:: ``filter_iterator`` models

      The concepts that ``filter_iterator`` models are dependent on which
      concepts the ``Iterator`` argument models, as specified in the
      following tables.

      +-----------------------------+----------------------------------------------------------+
      | If ``Iterator`` models      | then ``filter_iterator`` models                          |
      +=============================+==========================================================+
      | Single Pass Iterator        | Single Pass Iterator                                     |
      +-----------------------------+----------------------------------------------------------+
      | Forward Traversal Iterator  | Forward Traversal Iterator                               |
      +-----------------------------+----------------------------------------------------------+

      +--------------------------------+----------------------------------------------+
      | If ``Iterator`` models         | then ``filter_iterator`` models              |
      +================================+==============================================+
      | Readable Iterator              | Readable Iterator                            |
      +--------------------------------+----------------------------------------------+
      | Writable Iterator              | Writable Iterator                            |
      +--------------------------------+----------------------------------------------+
      | Lvalue Iterator                | Lvalue Iterator                              |
      +--------------------------------+----------------------------------------------+

      +-------------------------------------------------------+---------------------------------+
      | If ``Iterator`` models                                | then ``filter_iterator`` models |
      +=======================================================+=================================+
      | Readable Iterator, Single Pass Iterator               | Input Iterator                  |
      +-------------------------------------------------------+---------------------------------+
      | Readable Lvalue Iterator, Forward Traversal Iterator  | Forward Iterator                |
      +-------------------------------------------------------+---------------------------------+
      | Writable Lvalue Iterator, Forward Traversal Iterator  | Mutable Forward Iterator        |
      +-------------------------------------------------------+---------------------------------+


      ``filter_iterator<P1, X>`` is interoperable with ``filter_iterator<P2, Y>`` 
      if and only if ``X`` is interoperable with ``Y``.


    Change:
      :Returns: a ``filter_iterator`` whose
          predicate is a default constructed ``Predicate`` and
          whose ``end`` is a default constructed ``Iterator``.

    to:
      :Effects: Constructs a ``filter_iterator`` whose``m_pred``,  ``m_iter``, and ``m_end`` 
        members are a default constructed.

    Change:
      :Returns: A ``filter_iterator`` at position ``x`` that filters according
          to predicate ``f`` and that will not increment past ``end``.

    to:
      :Effects: Constructs a ``filter_iterator`` where ``m_iter`` is either
          the first position in the range ``[x,end)`` such that ``f(*m_iter) == true`` 
          or else``m_iter == end``. The member ``m_pred`` is constructed from
          ``f`` and ``m_end`` from ``end``.

    Change:
      :Returns: A ``filter_iterator`` at position ``x`` that filters 
          according to a default constructed ``Predicate``
          and that will not increment past ``end``.

    to:
      :Effects: Constructs a ``filter_iterator`` where ``m_iter`` is either
          the first position in the range ``[x,end)`` such that ``m_pred(*m_iter) == true`` 
          or else``m_iter == end``. The member ``m_pred`` is default constructed.


    Change:
      :Returns: A copy of iterator ``t``. 

    to:
      :Effects: Constructs a filter iterator whose members are copied from ``t``.

    Change:
      :Returns: A copy of the predicate object used to construct ``*this``.

    to:
      :Returns: ``m_pred``

    Change:
      :Returns: The object ``end`` used to construct ``*this``.

    to:
      :Returns: ``m_end``

    At the end of the operations section, add:

      ``reference operator*() const;``

      :Returns: ``*m_iter``


      ``filter_iterator& operator++();``

      :Effects: Increments ``m_iter`` and then continues to
        increment ``m_iter`` until either ``m_iter == m_end``
        or ``m_pred(*m_iter) == true``.
      :Returns: ``*this``  


    Change::

      class counting_iterator
        : public iterator_adaptor<
              counting_iterator<Incrementable, Access, Traversal, Difference>
            , Incrementable
            , Incrementable
            , Access
            , /* see details for traversal category */
            , Incrementable const&
            , Incrementable const*
            , /* distance = Difference or a signed integral type */>
      {
          friend class iterator_core_access;
       public:

    to::

      class counting_iterator
      {
       public:
          typedef Incrementable value_type;
          typedef const Incrementable& reference;
          typedef const Incrementable* pointer;
          typedef /* see below */ difference_type;
          typedef /* see below */ iterator_category;



    Change::

      private:
          typename counting_iterator::reference dereference() const
          {
              return this->base_reference();
          }

    to::

          Incrementable const& base() const;
          reference operator*() const;
          counting_iterator& operator++();
          counting_iterator& operator--();
      private:
          Incrementable m_inc; // exposition

    After the synopsis, add:

      If the ``Difference`` argument is ``use_default`` then
      ``difference_type`` is an unspecified signed integral
      type. Otherwise ``difference_type`` is ``Difference``.

      ``iterator_category`` is determined according to the following
      algorithm:

      .. parsed-literal::

         if (CategoryOrTraversal is not use_default)
             return CategoryOrTraversal
         else if (numeric_limits<Incrementable>::is_specialized)
             return |iterator-category|\ (
                 random_access_traversal_tag, Incrementable, const Incrementable&)
         else
             return |iterator-category|\ (
                  iterator_traversal<Incrementable>::type, 
                  Incrementable, const Incrementable&)



    Change:
      [*Note:* implementers are encouraged to provide an implementation of
        ``distance_to`` and a ``difference_type`` that avoids overflows in
        the cases when the ``Incrementable`` type is a numeric type.]

    to:
      [*Note:* implementers are encouraged to provide an implementation of
        ``operator-`` and a ``difference_type`` that avoid overflows in
        the cases where ``std::numeric_limits<Incrementable>::is_specialized``
        is true.]

    Change:
      The ``Incrementable`` type must be Default Constructible, Copy
      Constructible, and Assignable.  The default distance is
      an implementation defined signed integegral type.

      The resulting ``counting_iterator`` models Readable Lvalue Iterator.

    to:
      The ``Incrementable`` argument shall be Copy Constructible and Assignable.

    Change:
      Furthermore, if you wish to create a counting iterator that is a Forward
      Traversal Iterator, then the following expressions must be valid:

    to:
      If ``iterator_category`` is convertible to ``forward_iterator_tag``
      or ``forward_traversal_tag``, the following must be well-formed:

    Change:
      If you wish to create a counting iterator that is a 
      Bidirectional Traversal Iterator, then pre-decrement is also required:

    to:
      If ``iterator_category`` is convertible to
      ``bidirectional_iterator_tag`` or ``bidirectional_traversal_tag``,
      the following expression must also be well-formed:

    Change:
      If you wish to create a counting iterator that is a Random Access
      Traversal Iterator, then these additional expressions are also
      required:

    to:
      If ``iterator_category`` is convertible to
      ``random_access_iterator_tag`` or ``random_access_traversal_tag``,
      the following must must also be valid:

    After the requirements section, add:

.. topic:: ``counting_iterator`` models

      Specializations of ``counting_iterator`` model Readable Lvalue
      Iterator. In addition, they model the concepts corresponding to the
      iterator tags to which their ``iterator_category`` is convertible.
      Also, if ``CategoryOrTraversal`` is not ``use_default`` then
      ``counting_iterator`` models the concept corresponding to the iterator
      tag ``CategoryOrTraversal``.  Otherwise, if
      ``numeric_limits<Incrementable>::is_specialized``, then
      ``counting_iterator`` models Random Access Traversal Iterator.
      Otherwise, ``counting_iterator`` models the same iterator traversal
      concepts modeled by ``Incrementable``.

      ``counting_iterator<X,C1,D1>`` is interoperable with
      ``counting_iterator<Y,C2,D2>`` if and only if ``X`` is
      interoperable with ``Y``.

    At the begining of the operations section, add:

      In addition to the operations required by the concepts modeled by
      ``counting_iterator``, ``counting_iterator`` provides the following
      operations.

    Change:
      :Returns: A default constructed instance of ``counting_iterator``.

    to:
      :Requires: ``Incrementable`` is Default Constructible.
      :Effects: Default construct the member ``m_inc``.

    Change:
      :Returns: An instance of ``counting_iterator`` that is a copy of ``rhs``.

    to:
      :Effects: Construct member ``m_inc`` from ``rhs.m_inc``.

    Change:
      :Returns: An instance of ``counting_iterator`` with its base
          object copy constructed from ``x``.

    to:
      :Effects: Construct member ``m_inc`` from ``x``.

    At the end of the operations section, add:

      ``reference operator*() const;``

      :Returns: ``m_inc``


      ``counting_iterator& operator++();``

      :Effects: ``++m_inc``
      :Returns: ``*this``


      ``counting_iterator& operator--();``

      :Effects: ``--m_inc``
      :Returns: ``*this``  


      ``Incrementable const& base() const;``

      :Returns: ``m_inc``


9.38x Problem with specification of a->m in Readable Iterator
=============================================================

:Submitter: Howard Hinnant
:Status: New

c++std-lib-12585:

Readable Iterator Requirements says:

  +----------+--------+-----------------------------------------------------------+
  | ``a->m`` | ``U&`` | pre: ``(*a).m`` is well-defined. Equivalent to ``(*a).m`` |
  +----------+--------+-----------------------------------------------------------+

Do we mean to outlaw iterators with proxy references from meeting
the readable requirements?

Would it be better for the requirements to read ``static_cast<T>(*a).m``
instead of ``(*a).m`` ?

:Proposed resolution: NAD.  

:Rationale: We think you're misreading "pre:".
  If ``(*a).m`` is not well defined, then the iterator is not
  required to provide ``a->m``. So a proxy iterator is not
  required to provide ``a->m``.

  As an aside, it is possible for proxy iterators to
  support ``->``, so changing the requirements to
  read ``static_cast<T>(*a).m`` is interesting.
  However, such a change to Readable Iterator would
  mean that it no longer corresponds to the
  input iterator requirements. So old iterators would not
  necessarily conform to new iterator requirements. 


9.39x counting_iterator Traversal argument unspecified
======================================================

:Submitter: Pete Becker

c++std-lib-12635:
 
counting_iterator takes an argument for its Traversal type, with a
default value of use_default. It is derived from an instance of
iterator_adaptor, where the argument passed for the Traversal type
is described as "\ ``/* see details for traversal category
*/``". The details for counting_iterator describe constraints on
the Incrementable type imposed by various traversal
categories. There is no description of what the argument to
iterator_adaptor should be.


:Proposed resolution:
  We no longer inherit from iterator_adaptor. So instead,
  we specify the iterator_category in terms of the Traversal type
  (which is now called CategoryOrTraversal). Also the
  requirements and models section was reorganized to
  match these changes and to make more sense.



9.40x indirect_iterator requirements muddled
============================================

:Submitter: Pete Becker

c++std-lib-12640:

    The value_type of the Iterator template parameter should itself
    be dereferenceable. The return type of the ``operator*`` for
    the value_type must be the same type as the Reference template
    parameter.

  I'd say this a bit differently, to emphasize what's required:
  iterator_traits<Iterator>::value_type must be dereferenceable.
  The Reference template parameter must be the same type as 
  ``*iterator_traits<Iterator>::value_type()``.

    The Value template parameter will be the value_type for the 
    indirect_iterator, unless Value is const. If Value is const X, then 
    value_type will be non- const X.

  Also non-volatile, right? In other words, if Value isn't use_default, it 
  just gets passed as the Value argument for iterator_adaptor.

    The default for Value is::

      iterator_traits< iterator_traits<Iterator>::value_type >::value_type

    If the default is used for Value, then there must be a valid
    specialization of iterator_traits for the value type of the
    base iterator.

  The earlier requirement is that
  ``iterator_traits<Iterator>::value_type`` must be
  dereferenceable. Now it's being treated as an iterator. Is this
  just a pun, or is ``iterator_traits<Iterator>::value_type``
  required to be some form of iterator? If it's the former we need
  to find a different way to say it.  If it's the latter we need to
  say so.

:Proposed resolution:
  Change:

    The ``value_type`` of the ``Iterator`` template parameter
    should itself be dereferenceable. The return type of the
    ``operator*`` for the ``value_type`` must be the same type as
    the ``Reference`` template parameter. The ``Value`` template
    parameter will be the ``value_type`` for the
    ``indirect_iterator``, unless ``Value`` is const. If ``Value``
    is ``const X``, then ``value_type`` will be *non-* ``const X``.
    The default for ``Value`` is::

      iterator_traits< iterator_traits<Iterator>::value_type >::value_type

    If the default is used for ``Value``, then there must be a
    valid specialization of ``iterator_traits`` for the value type
    of the base iterator.

    The ``Reference`` parameter will be the ``reference`` type of the
    ``indirect_iterator``. The default is ``Value&``.

    The ``Access`` and ``Traversal`` parameters are passed
    unchanged to the corresponding parameters of the
    ``iterator_adaptor`` base class, and the ``Iterator`` parameter
    is passed unchanged as the ``Base`` parameter to the
    ``iterator_adaptor`` base class.

  to:

    The expression ``*v``, where ``v`` is an object of
    ``iterator_traits<Iterator>::value_type``, shall be valid
    expression and convertible to ``reference``.  ``Iterator``
    shall model the traversal concept indicated by
    ``iterator_category``.  ``Value``, ``Reference``, and
    ``Difference`` shall be chosen so that ``value_type``,
    ``reference``, and ``difference_type`` meet the requirements
    indicated by ``iterator_category``.

   [Note: there are further requirements on the
   ``iterator_traits<Iterator>::value_type`` if the ``Value``
   parameter is not ``use_default``, as implied by the algorithm
   for deducing the default for the ``value_type`` member.]


:Rationale: Not included above is the specification of the
  ``value_type``, ``reference``, etc., members, which is handled by
  the changes in 9.37x.


9.41x Problem with transform_iterator requirements
==================================================

:Submitter: Pete Becker

c++std-lib-12641:

      The reference type of transform_iterator is ``result_of<
      UnaryFunction(iterator_traits<Iterator>::reference)
      >::type``. The ``value_type`` is
      ``remove_cv<remove_reference<reference> >::type``.

These are the defaults, right? If the user supplies their own types
that's what gets passed to iterator_adaptor. And again, the
specification should be in terms of the specialization of
iterator_adaptor, and not in terms of the result:

Reference argument to iterator_adaptor::

    if (Reference != use_default)
        Reference
    else
        result_of<
            UnaryFunction(iterator_traits<Iterator>::reference)
        >::type

Value argument to iterator_adaptor::

    if (Value != use_default)
        Value
    else if (Reference != use_default)
        remove_reference<reference>::type
    else
        remove_reference<
            result_of<
                UnaryFunction(iterator_traits<Iterator>::reference)
            >::type
        >::type

There's probably a better way to specify that last alternative, but
I've been at this too long, and it's all turning into a maze of
twisty passages, all alike.

:Proposed resolution:
  Replace:

      The reference type of transform_iterator is ``result_of<
      UnaryFunction(iterator_traits<Iterator>::reference)
      >::type``. The ``value_type`` is
      ``remove_cv<remove_reference<reference> >::type``.

  with:

      If ``Reference`` is ``use_default`` then the ``reference``
      member of ``transform_iterator`` is ``result_of<
      UnaryFunction(iterator_traits<Iterator>::reference)
      >::type``.  Otherwise, ``reference`` is ``Reference``.

      If ``Value`` is ``use_default`` then the ``value_type``
      member is ``remove_cv<remove_reference<reference> >::type``.
      Otherwise, ``value_type`` is ``Value``.


9.42x filter_iterator details unspecified
=========================================

:Submitter: Pete Becker

c++std-lib-12642:

The paper says::

    template<class Predicate, class Iterator>
    class filter_iterator
         : public iterator_adaptor<
             filter_iterator<Predicate, Iterator>,
             Iterator,
             use_default,
             /* see details */ >
 
That comment covers the Access, Traversal, Reference, and Difference
arguments. The only specification for any of these in the details is:
 
    The access category of the filter_iterator will be the same as
    the access category of Iterator.
 
Needs more.

:Proposed resolution:
  Add to the synopsis::

      typedef iterator_traits<Iterator>::value_type value_type;
      typedef iterator_traits<Iterator>::reference reference;
      typedef iterator_traits<Iterator>::pointer pointer;
      typedef iterator_traits<Iterator>::difference_type difference_type;
      typedef /* see below */ iterator_category;

  and add just after the synopsis:

      If ``Iterator`` models Readable Lvalue Iterator and Forward
      Traversal Iterator then ``iterator_category`` is convertible
      to ``std::forward_iterator_tag``. Otherwise
      ``iterator_category`` is convertible to
      ``std::input_iterator_tag``.


9.43x transform_iterator interoperability too restrictive
=========================================================

:Submitter: Jeremy Siek

We do not need to require that the function objects have the same
type, just that they be convertible.

:Proposed resolution:

  Change::

      template<class OtherIterator, class R2, class V2>
      transform_iterator(
            transform_iterator<UnaryFunction, OtherIterator, R2, V2> const& t
          , typename enable_if_convertible<OtherIterator, Iterator>::type* = 0 // exposition
      );

  to::

      template<class F2, class I2, class R2, class V2>
      transform_iterator(
            transform_iterator<F2, I2, R2, V2> const& t
          , typename enable_if_convertible<I2, Iterator>::type* = 0      // exposition only
          , typename enable_if_convertible<F2, UnaryFunction>::type* = 0 // exposition only
      );


9.44y ``indirect_iterator`` and smart pointers
==============================================

:Submitter: Dave Abrahams

``indirect_iterator`` should be able to iterate over containers of
smart pointers, but the mechanism that allows it was left out of
the specification, even though it's present in the Boost
specification

:Proposed resolution: Add ``pointee`` and ``indirect_reference``
  to deal with this capability.

  In [lib.iterator.helper.synopsis], add::

    template <class Dereferenceable>
    struct pointee;

    template <class Dereferenceable>
    struct indirect_reference;

  After ``indirect_iterator``\ 's abstract, add:

.. topic:: Class template ``pointee``

  .. include:: pointee_ref.rst

.. topic:: Class template ``indirect_reference``

  .. include:: indirect_reference_ref.rst

See proposed resolution to Issue 9.37x for more changes related to
this issue.

9.45y N1530: Typos and editorial changes in proposal text (not standardese)
===========================================================================

:Submitter: Dave Abrahams

1. "because specification helps to highlight that the ``Reference``
   template parameter may not always be identical to the iterator's
   ``reference`` type, and will keep users making mistakes based on
   that assumption."

   :Proposed resolution: add "from" before "making"

2. mention of obsolete projection_iterator

  :Proposed Resolution:

    From n1530, in the **Specialized Adaptors** section, remove:

      ``projection_iterator``, which is similar to ``transform_iterator``
      except that when dereferenced it returns a reference instead of
      a value.

  :Rationale:
    This iterator was in the original boost library, but the new
    iterator concepts allowed this iterator to be
    folded into ``transform_iterator``.



9.46y N1530: ``base()`` return-by-value is costly
=================================================

:Submitter: Dave Abrahams

We've had some real-life reports that iterators that use
``iterator_adaptor``\ 's ``base()`` function can be inefficient
when the ``Base`` iterator is expensive to copy.  Iterators, of
all things, should be efficient.

:Proposed resolution: 

   In [lib.iterator.adaptor]

   Change::

      Base base() const;

   to::

      Base const& base() const;

   twice (once in the synopsis and once in the **public
   operations** section).


9.47x Forgot default constructible in Forward Traversal Iterator
================================================================

:Submitter: Jeremy Siek

We want Forward Traversal Iterator plus Readable Lvalue Iterator to
match the old Foward Iterator requirements, so we need Forward
Traversal Iterator to include Default Constructible.

:Proposed resolution: 

  Change:

    A class or built-in type ``X`` models the *Forward Traversal Iterator*
    concept if the following expressions are valid and respect the stated
    semantics.

    +-------------------------------------------------------------------------------------------+
    |Forward Traversal Iterator Requirements (in addition to Single Pass Iterator)              |
    +---------------------------------------+-----------------------------------+---------------+

  to:

    A class or built-in type ``X`` models the *Forward Traversal Iterator*
    concept if, in addition to ``X`` meeting the requirements of
    Default Constructible and Single Pass Iterator, the following
    expressions are valid and respect the
    stated semantics.

    +--------------------------------------------------------------------------------------------------------+
    |Forward Traversal Iterator Requirements (in addition to Default Constructible and Single Pass Iterator) |
    +---------------------------------------+-----------------------------------+----------------------------+
        
9.48x Editorial changes (non-normative text)
============================================

Change:
  Iterator facade uses the Curiously Recurring Template Pattern (CRTP)
  [Cop95] so that the user can specify the behavior of
  ``iterator_facade`` in a derived class.  Former designs used policy
  objects to specify the behavior.  ``iterator_facade`` does not use policy
  objects for several reasons: 

to:
  Iterator facade uses the Curiously Recurring Template
  Pattern (CRTP) [Cop95] so that the user can specify the behavior
  of ``iterator_facade`` in a derived class.  Former designs used
  policy objects to specify the behavior, but that approach was
  discarded for several reasons:


Change:
  iterator's ``operator++`` returns the iterator type itself means
  that all iterators generated by ``iterator_facade`` would be
  instantiations of ``iterator_facade``.  Cumbersome type generator

to:
  iterator's ``operator++`` returns the iterator type itself
  would mean that all iterators built with the library would
  have to be specializations of ``iterator_facade<...>``, rather
  than something more descriptive like
  ``indirect_iterator<T*>``.  Cumbersome type generator


Change:
  The return type for ``operator->`` and ``operator[]`` is not
  explicitly specified. Instead it requires each ``iterator_facade``
  instantiation to meet the requirements of its ``iterator_category``.

To:
  The return types for ``iterator_facade``\ 's ``operator->`` and
  ``operator[]`` are not explicitly specified. Instead, those types
  are described in terms of a set of requirements, which must be
  satisfied by the ``iterator_facade`` implementation.


9.49x Clarification of iterator_facade requirements and type members
====================================================================

A general cleanup and simplification of the requirements and
description of type members for ``iterator_facade``.


The user is only allowed to add ``const`` as a qualifier.

Change:
  ``typedef remove_cv<Value>::type value_type;``

to:
  ``typedef remove_const<Value>::type value_type;``


We use to have an unspecified type for ``pointer``, to match the
return type of ``operator->``, but there's no real reason to make them
match, so we just use the simpler ``Value*`` for ``pointer``.

Change:

  ``typedef /* see description of operator-> */ pointer;``

To:
  ``typedef Value* pointer;``


Remove:
  Some of the constraints on template parameters to
  ``iterator_facade`` are expressed in terms of resulting nested
  types and should be viewed in the context of their impact on
  ``iterator_traits<Derived>``.

Change:
  The ``Derived`` template parameter must be a class derived from
  ``iterator_facade``.

and:
  The following table describes the other requirements on the
  ``Derived`` parameter.  Depending on the resulting iterator's
  ``iterator_category``, a subset of the expressions listed in the table
  are required to be valid.  The operations in the first column must be
  accessible to member functions of class ``iterator_core_access``.

to:
  The following table describes the typical valid expressions on
  ``iterator_facade``\ 's ``Derived`` parameter, depending on the
  iterator concept(s) it will model.  The operations in the first
  column must be made accessible to member functions of class
  ``iterator_core_access``.  In addition,
  ``static_cast<Derived*>(iterator_facade*)`` shall be well-formed.


Remove:
  The nested ``::value_type`` type will be the same as
  ``remove_cv<Value>::type``, so the ``Value`` parameter must be
  an (optionally ``const``\ -qualified) non-reference type.

  The nested ``::reference`` will be the same as the ``Reference``
  parameter; it must be a suitable reference type for the resulting
  iterator.  The default for the ``Reference`` parameter is
  ``Value&``.


Change:

  In the table below, ``X`` is the derived iterator type, ``a`` is an
  object of type ``X``, ``b`` and ``c`` are objects of type ``const X``,
  ``n`` is an object of ``X::difference_type``, ``y`` is a constant
  object of a single pass iterator type interoperable with X, and ``z``
  is a constant object of a random access traversal iterator type
  interoperable with ``X``.

  +--------------------+-------------------+-------------------------------------+---------------------------+
  |Expression          |Return Type        |Assertion/Note                       |Required to implement      |
  |                    |                   |                                     |Iterator Concept(s)        |
  +====================+===================+=====================================+===========================+
  |``c.dereference()`` |``X::reference``   |                                     |Readable Iterator, Writable|
  |                    |                   |                                     |Iterator                   |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``c.equal(b)``      |convertible to bool|true iff ``b`` and ``c`` are         |Single Pass Iterator       |
  |                    |                   |equivalent.                          |                           |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``c.equal(y)``      |convertible to bool|true iff ``c`` and ``y`` refer to the|Single Pass Iterator       |
  |                    |                   |same position.  Implements ``c == y``|                           |
  |                    |                   |and ``c != y``.                      |                           |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``a.advance(n)``    |unused             |                                     |Random Access Traversal    |
  |                    |                   |                                     |Iterator                   |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``a.increment()``   |unused             |                                     |Incrementable Iterator     |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``a.decrement()``   |unused             |                                     |Bidirectional Traversal    |
  |                    |                   |                                     |Iterator                   |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``c.distance_to(b)``|convertible to     |equivalent to ``distance(c, b)``     |Random Access Traversal    |
  |                    |X::difference_type |                                     |Iterator                   |
  +--------------------+-------------------+-------------------------------------+---------------------------+
  |``c.distance_to(z)``|convertible to     |equivalent to ``distance(c, z)``.    |Random Access Traversal    |
  |                    |X::difference_type |Implements ``c - z``, ``c < z``, ``c |Iterator                   |
  |                    |                   |<= z``, ``c > z``, and ``c >= c``.   |                           |
  +--------------------+-------------------+-------------------------------------+---------------------------+

to:

  In the table below, ``F`` is ``iterator_facade<X,V,C,R,D>``, ``a`` is an
  object of type ``X``, ``b`` and ``c`` are objects of type ``const X``,
  ``n`` is an object of ``F::difference_type``, ``y`` is a constant
  object of a single pass iterator type interoperable with ``X``, and ``z``
  is a constant object of a random access traversal iterator type
  interoperable with ``X``.

  **iterator_facade Core Operations**

  +--------------------+----------------------+-------------------------+---------------------------+
  |Expression          |Return Type           |Assertion/Note           |Used to implement Iterator |
  |                    |                      |                         |Concept(s)                 |
  +====================+======================+=========================+===========================+
  |``c.dereference()`` |``F::reference``      |                         |Readable Iterator, Writable|
  |                    |                      |                         |Iterator                   |
  +--------------------+----------------------+-------------------------+---------------------------+
  |``c.equal(y)``      |convertible to bool   |true iff ``c`` and ``y`` |Single Pass Iterator       |
  |                    |                      |refer to the same        |                           |
  |                    |                      |position.                |                           |
  +--------------------+----------------------+-------------------------+---------------------------+
  |``a.increment()``   |unused                |                         |Incrementable Iterator     |
  +--------------------+----------------------+-------------------------+---------------------------+
  |``a.decrement()``   |unused                |                         |Bidirectional Traversal    |
  |                    |                      |                         |Iterator                   |
  +--------------------+----------------------+-------------------------+---------------------------+
  |``a.advance(n)``    |unused                |                         |Random Access Traversal    |
  |                    |                      |                         |Iterator                   |
  +--------------------+----------------------+-------------------------+---------------------------+
  |``c.distance_to(z)``|convertible to        |equivalent to            |Random Access Traversal    |
  |                    |``F::difference_type``|``distance(c, X(z))``.   |Iterator                   |
  +--------------------+----------------------+-------------------------+---------------------------+



