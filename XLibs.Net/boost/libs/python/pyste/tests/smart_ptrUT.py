import unittest
from _smart_ptr import *

class BasicExampleTest(unittest.TestCase):

    def testIt(self):
        c = NewC()
        d = NewD()
        c.value = 3
        d.Set(c)
        c1 = d.Get()
        c1.value = 6
        self.assertEqual(c.value, 6)
        a = NewA()
        self.assertEqual(GetA(a), 1)

if __name__ == '__main__':
    unittest.main()     
