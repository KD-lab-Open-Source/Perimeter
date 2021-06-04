import unittest
from _add_test import *

class AddMethodTest(unittest.TestCase):

    def testIt(self):
        c = C()
        c.x = 10
        self.assertEqual(c.get_x(), 10)

if __name__ == '__main__':
    unittest.main()
