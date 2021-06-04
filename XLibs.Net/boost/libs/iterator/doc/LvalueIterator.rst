Lvalue Iterator Concept
.......................

The *Lvalue Iterator* concept adds the requirement that the return
type of ``operator*`` type be a reference to the value type of the
iterator.

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
