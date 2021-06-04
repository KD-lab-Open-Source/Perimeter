import re
import sys
import os
import string

def tidy_attr(attr_value):
    return string.lower(attr_value.group())
    
def tidy(file):
    lines = open(file, 'r').readlines()
    regex = re.compile(r'[name|class]="([^"]*)"')
    xsltproc_patch = re.compile(r'(\|\|\|)')
    for i in range(len(lines)):
        lines[i] = regex.sub(tidy_attr,lines[i])
        lines[i] = xsltproc_patch.sub(' ',lines[i])
    
    open(file, 'w') \
        .writelines(lines)

def main():
    if len(sys.argv) < 2:
        print '\tusage: %s <html document>' % sys.argv[0]
        return -1

    tidy(sys.argv[1])
    print 'done!'

main()
