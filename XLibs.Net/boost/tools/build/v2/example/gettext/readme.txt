
This example shows how it's possible to used GNU gettext utilities with
Boost.Build.

A simple translation file is compiled and installed as message catalog for
russian. The main application explicitly switches to russian locale and
output the translation of "hello".

To test:

   bjam
   bin/gcc/debug/main

To test even more:

   - add more localized strings to "main.cpp"
   - run "bjam update-russian"
   - edit "russian.po"
   - run bjam
   - run "main"

