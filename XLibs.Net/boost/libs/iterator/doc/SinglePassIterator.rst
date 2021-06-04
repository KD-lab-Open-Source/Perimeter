Single Pass Iterator Concept
............................

A class or built-in type ``X`` models the *Single Pass Iterator*
concept if the following expressions are valid and respect the stated
semantics.


+------------------------------------------------------------------------------------------+
|Single Pass Iterator Requirements (in addition to Incrementable Iterator and Equality     |
|Comparable)                                                                               |
+--------------------------------+-----------------------------+---------------------------+
|Expression                      |Return Type                  |Assertion/Semantics /      | 
|                                |                             |Pre-/Post-condition        |
+================================+=============================+===========================+
|``++r``                         |``X&``                       |pre: ``r`` is              |
|                                |                             |dereferenceable; post:     |
|                                |                             |``r`` is dereferenceable or|
|                                |                             |``r`` is past-the-end      |
+--------------------------------+-----------------------------+---------------------------+
|``a == b``                      |convertible to ``bool``      |``==`` is an equivalence   |
|                                |                             |relation over its domain   |
+--------------------------------+-----------------------------+---------------------------+
|``a != b``                      |convertible to ``bool``      |``!(a == b)``              |
+--------------------------------+-----------------------------+---------------------------+
|``iterator_traversal<X>::type`` |Convertible to               |                           |
|                                |``single_pass_traversal_tag``|                           |
+--------------------------------+-----------------------------+---------------------------+
