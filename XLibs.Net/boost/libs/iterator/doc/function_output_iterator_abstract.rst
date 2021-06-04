The function output iterator adaptor makes it easier to create custom
output iterators. The adaptor takes a unary function and creates a
model of Output Iterator. Each item assigned to the output iterator is
passed as an argument to the unary function.  The motivation for this
iterator is that creating a conforming output iterator is non-trivial,
particularly because the proper implementation usually requires a
proxy object.

