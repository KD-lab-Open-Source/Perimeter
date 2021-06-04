import unittest
from _opaque import *

class OpaqueTest(unittest.TestCase):

    def testIt(self):

        c = new_C()
        self.assertEqual(get(c), 10)
        c = new_C_zero()
        self.assertEqual(get(c), 0) 
        a = A()
        d = a.new_handle()
        self.assertEqual(a.get(d), 3.0)
        self.assertEqual(a.f(), 0)
        self.assertEqual(a.f(3), 3)


if __name__ == '__main__':
    unittest.main()
