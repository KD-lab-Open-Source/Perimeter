# Copyright Bruno da Silva de Oliveira 2003. Use, modification and 
# distribution is subject to the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at 
# http:#www.boost.org/LICENSE_1_0.txt)

from Exporter import Exporter
from settings import *
import utils

#==============================================================================
# EnumExporter 
#==============================================================================
class EnumExporter(Exporter):
    'Exports enumerators'

    def __init__(self, info):
        Exporter.__init__(self, info)


    def SetDeclarations(self, declarations):
        Exporter.SetDeclarations(self, declarations)
        if self.declarations:
            self.enum = self.GetDeclaration(self.info.name)
        else:
            self.enum = None

    def Export(self, codeunit, exported_names):
        if not self.info.exclude:
            indent = self.INDENT
            in_indent = self.INDENT*2
            rename = self.info.rename or self.enum.name
            full_name = self.enum.FullName()
            unnamed_enum = False
            if rename.startswith('$_') or rename.startswith('._'):
                unique_number = hash(self.info.include)
                unnamed_enum = True
                self.ExportUniqueInt(codeunit)
                full_name = namespaces.pyste + 'UniqueInt<%d>' % unique_number
                rename = "unnamed"
            code = indent + namespaces.python
            code += 'enum_< %s >("%s")\n' % (full_name, rename)
            for name in self.enum.values:         
                rename = self.info[name].rename or name
                value_fullname = self.enum.ValueFullName(name)
                code += in_indent + '.value("%s", %s)\n' % (rename, value_fullname)
            if self.info.export_values or unnamed_enum:
                code += in_indent + '.export_values()\n'
            code += indent + ';\n\n'
            codeunit.Write('module', code)
            exported_names[self.enum.FullName()] = 1


    def ExportUniqueInt(self, codeunit):
        write = lambda s: codeunit.Write('declaration', s)
        write('// Unique type for unnamed enums\n')
        write('#ifndef PYSTE_UNIQUE_INT_DEFINED\n')
        write('#define PYSTE_UNIQUE_INT_DEFINED\n')
        write('template<int num>\n')
        write('struct UniqueInt {\n')
        write('   int v;\n')
        write('   enum { value=num };\n')
        write('   UniqueInt(int v_):\n') 
        write('       v(v_)\n')
        write('   {}\n')
        write('   operator int() const\n')
        write('   { return v; }\n')
        write('};\n')
        write('#endif // PYSTE_UNIQUE_INT_DEFINED \n')
            

    def Name(self):
        return self.info.name
