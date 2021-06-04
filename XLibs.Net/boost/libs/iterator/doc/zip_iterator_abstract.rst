The zip iterator provides the ability to parallel-iterate
over several controlled sequences simultaneously. A zip 
iterator is constructed from a tuple of iterators. Moving
the zip iterator moves all the iterators in parallel.
Dereferencing the zip iterator returns a tuple that contains
the results of dereferencing the individual iterators. 
