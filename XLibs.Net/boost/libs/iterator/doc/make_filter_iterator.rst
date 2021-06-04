
::

    template <class Predicate, class Iterator>
    filter_iterator<Predicate,Iterator>
    make_filter_iterator(Predicate f, Iterator x, Iterator end = Iterator());

:Returns: filter_iterator<Predicate,Iterator>(f, x, end)

::

    template <class Predicate, class Iterator>
    filter_iterator<Predicate,Iterator>
    make_filter_iterator(Iterator x, Iterator end = Iterator());

:Returns: filter_iterator<Predicate,Iterator>(x, end)
