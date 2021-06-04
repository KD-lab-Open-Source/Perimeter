::

    template <class UnaryFunction, class Iterator>
    transform_iterator<UnaryFunction, Iterator>
    make_transform_iterator(Iterator it, UnaryFunction fun);

:Returns: An instance of ``transform_iterator<UnaryFunction, Iterator>`` with ``m_f``
  initialized to ``f`` and ``m_iterator`` initialized to ``x``.



::

    template <class UnaryFunction, class Iterator>
    transform_iterator<UnaryFunction, Iterator>
    make_transform_iterator(Iterator it);

:Returns: An instance of ``transform_iterator<UnaryFunction, Iterator>`` with ``m_f``
  default constructed and ``m_iterator`` initialized to ``x``.
