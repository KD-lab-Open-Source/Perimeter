'''
>>> from implicit_ext import *
>>> x_value(X(42))
42
>>> x_value(42)
42
>>> x = make_x(X(42))
>>> x.value()
42
>>> try: make_x('fool')
... except TypeError: pass
... else: print 'no error'
'''

def run(args = None):
    import sys
    import doctest

    if args is not None:
        sys.argv = args
    return doctest.testmod(sys.modules.get(__name__))
    
if __name__ == '__main__':
    print "running..."
    import sys
    status = run()[0]
    if (status == 0): print "Done."
    sys.exit(status)
