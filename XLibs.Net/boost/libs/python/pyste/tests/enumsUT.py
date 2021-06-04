import unittest
from _enums import *

class EnumsTest(unittest.TestCase):

    def testIt(self):
        self.assertEqual(int(Red), 0)
        self.assertEqual(int(Blue), 1)

        self.assertEqual(int(X.Choices.Good), 1)
        self.assertEqual(int(X.Choices.Bad), 2)
        a = X()
        self.assertEqual(a.set(a.Choices.Good), 1)
        self.assertEqual(a.set(a.Choices.Bad), 2)
        self.assertEqual(x, 0)
        self.assertEqual(y, 1)


if __name__ == '__main__':
    unittest.main()
