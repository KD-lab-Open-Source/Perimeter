The filter iterator adaptor creates a view of an iterator range in
which some elements of the range are skipped. A predicate function
object controls which elements are skipped. When the predicate is
applied to an element, if it returns ``true`` then the element is
retained and if it returns ``false`` then the element is skipped
over. When skipping over elements, it is necessary for the filter
adaptor to know when to stop so as to avoid going past the end of the
underlying range. A filter iterator is therefore constructed with pair
of iterators indicating the range of elements in the unfiltered
sequence to be traversed.

