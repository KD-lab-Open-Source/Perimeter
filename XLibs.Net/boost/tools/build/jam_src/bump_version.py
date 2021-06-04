#!/usr/bin/python

# This script is used to bump version of bjam. It takes a single argument, e.g
#
#    ./bump_version.py 3.1.9
#
# and updates all necessary files. For the time being, it's assumes presense
# of 'perl' executable and Debian-specific 'dch' executable.
#
 

import sys
import string
import os

def spec(version):
    os.system("perl -pi -e 's|^Version:.*|Version: %s|' boost-jam.spec" %
              string.join(version, "."))

def build_jam(version):
    os.system("perl -pi -e 's|^VERSION = .* ;|VERSION = %s\$(.)%s\$(.)%s ;|' build.jam"
              % (version[0], version[1], version[2]))

def index_html(version):
    os.system("perl -pi -e 's|This is version .* of BJam|This is version %s of BJam|' index.html"
              % string.join(version, "."))

def jam_c(version):
    re = "\\*major_version = .*, \\*minor_version = .*, \\*changenum = .*";
    new = ('*major_version = "%02d", *minor_version = "%02d", *changenum = "%02d";' %
        (int(version[0]), int(version[1]), int(version[2])))
    os.system("perl -pi -e 's|%s|%s|' jam.c" % (re, new))

def patchlevel(version):
    os.system("perl -pi -e 's|VERSION .*|VERSION \"%s\"|' patchlevel.h" %
              string.join(version, "."))

def dch(version):
    os.system("dch --ignore-dirname -v " + string.join(version, ".") + "-1")
       
bumpers = [spec, build_jam, index_html, jam_c, patchlevel, dch]

def main():

    if len(sys.argv) < 2:
        print "Expect new version as argument"
        sys.exit(1)
                
    new_version = string.split(sys.argv[1], ".")
    print "Setting version to", new_version
    for b in bumpers:
        b(new_version)

if __name__ == '__main__':
    main()
