#!/usr/bin/python

# qtcreator gdb dumper
def qdump__ACE_TCHAR(d, value):
    cstring = self.extractCString(value[0], 0)
    d.put('addr="%x", ' % value.address +
          'value="%s",' % cstring +
          'type="ACE_TCHAR[%d]",' % value.size +
          'numchild="0"')


# gdb pretty printer
import gdb
#import gdb.printing
#import itertools
import re
#import sys

class ACE_TCHARArrayPrinter(object):
         """print a ACE_TCHAR[]"""

         def __init__(self, val):
             print ('found ACE_TCHAR[]')
             self.val = val

         def to_string(self):
             print ('ACE_TCHARArrayPrinter::to_string')
             return self.val

         def display_hint(self):
             return 'string'

def lookup_acetchararray(val):
         lookup_tag = val.type.tag
         if lookup_tag == None:
             return None
         regex = re.compile("^ACE_TCHAR\[(\\d)*\]$")
         if regex.match(lookup_tag):
             return ACE_TCHARArrayPrinter(val)
         return None

def lookup(value):
         "pretty-printer map"
         code = value.type.code
#         typename = str(value.type.name)
         typename = str(value.type.strip_typedefs().unqualified())
         tag = str(value.type.tag)
         target = str(value.type.target())
         print ('code=',code)
         print ('typename=',typename)
         print ('tag=',tag)
         print ('target=',target)

         if typename == 'ACE_TCHAR' and code == Gdb.TYPE_CODE_ARRAY:
             return ACE_TCHARArrayPrinter(value)

         return None

#gdb.pretty_printers.append (lookup_acetchararray)
#gdb.pretty_printers['^ACE_TCHAR\[(\\d)*\]$'] = ACE_TCHARArrayPrinter
gdb.pretty_printers.append (lookup)

#def register_printers(objfile):
#         objfile.pretty_printers.append(acetchar_array_lookup_function)

# using gdb.printing
#def build_pretty_printer():
#         pp = gdb.printing.RegexpCollectionPrettyPrinter(
#             "libcommon")
#         pp.add_printer('ACE_TCHAR', '^ACE_TCHAR\[\\d\]$', ACE_TCHARArrayPrinter)
#         return pp
