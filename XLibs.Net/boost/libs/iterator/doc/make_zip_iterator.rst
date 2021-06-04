::

    template<typename IteratorTuple> 
    zip_iterator<IteratorTuple> 
    make_zip_iterator(IteratorTuple t);

:Returns: An instance of ``zip_iterator<IteratorTuple>`` with ``m_iterator_tuple``
  initialized to ``t``.
