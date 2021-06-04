import unittest
from _code_test import *

class CodeTest(unittest.TestCase):

    def testIt(self):
        a = A()
        a.x = 12
        self.assertEqual(get(a), 12)
        self.assertEqual(foo(), "Hello!")


if __name__ == '__main__':
    unittest.main()
