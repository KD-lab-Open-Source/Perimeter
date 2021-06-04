
Readable Iterator Concept
.........................

A class or built-in type ``X`` models the *Readable Iterator* concept
for value type ``T`` if, in addition to ``X`` being Assignable and
Copy Constructible, the following expressions are valid and respect
the stated semantics. ``U`` is the type of any specified member of
type ``T``.

+-----------------------------------------------------------------------------------------------------------------------------+
|Readable Iterator Requirements (in addition to Assignable and Copy Constructible)                                            |
+-----------------------------------+------------------------+----------------------------------------------------------------+
|Expression                         |Return Type             |Note/Precondition                                               |
+===================================+========================+================================================================+
|``iterator_traits<X>::value_type`` |``T``                   |Any non-reference,                                              |
|                                   |                        |non-cv-qualified type                                           |
+-----------------------------------+------------------------+----------------------------------------------------------------+
|``*a``                             | Convertible to ``T``   |pre: ``a`` is dereferenceable. If ``a == b`` then ``*a``        |
|                                   |                        | is equivalent to ``*b``.                                       |
+-----------------------------------+------------------------+----------------------------------------------------------------+
|``a->m``                           |``U&``                  |pre: ``pre: (*a).m`` is well-defined. Equivalent to ``(*a).m``. |
+-----------------------------------+------------------------+----------------------------------------------------------------+
