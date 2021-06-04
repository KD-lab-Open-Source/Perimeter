# Copyright (C) 2003 Rational Discovery LLC
# Permission to copy, use, modify, sell and distribute this software
# is granted provided this copyright notice appears in all
# copies. This software is provided "as is" without express or
# implied warranty, and with no claim as to its suitability for any
# purpose.

print "running..."

import crossmod_exception_a
import crossmod_exception_b

try:
  crossmod_exception_b.tossit()
except IndexError:
  pass
try:
  crossmod_exception_a.tossit()
except IndexError:
  pass

print "Done."
