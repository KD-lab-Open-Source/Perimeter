from distutils.core import setup
import py2exe
import sys

sys.path.append('../src')
setup(name='pyste', scripts=['../src/pyste.py'])
