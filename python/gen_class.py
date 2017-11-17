from sys import argv
from FileWriter import *


def get_pre_compile_header(str):
    str = str.upper()
    str = str.replace(".", "_", 1)
    return '__' + str + '__'


def gen_singlton_class(class_name):
    h_file = class_name + '.h'
    cpp_file = class_name + '.cpp'

    precompile_header = get_pre_compile_header(h_file)

    # .h
    _h_file = FileWriter(h_file, 'w')
    _h_file.fprint('#ifndef ' + precompile_header)
    _h_file.fprint('#define ' + precompile_header)
    _h_file.fprint('\nclass ' + class_name)
    _h_file.fprint('{')
    _h_file.fprint('public:')
    _h_file.fprint('\tstatic ' + class_name + '* getInstance();')
    _h_file.fprint('protected:')
    _h_file.fprint('\t' + class_name + '();')
    _h_file.fprint('};\n')
    _h_file.fprint('#endif //' + precompile_header)

    # .cpp
    _cpp_file = FileWriter(cpp_file, 'w')

    # out getInstance()
    _cpp_file.fprint('#include "' + h_file + '"\n')
    _cpp_file.fprint(class_name + '* ' + class_name + '::getInstance()')
    _cpp_file.fprint('{')
    _cpp_file.fprint('\tstatic ' + class_name + ' instance;')
    _cpp_file.fprint('\treturn &instance;')
    _cpp_file.fprint('}\n')

    # out ctor
    _cpp_file.fprint(class_name + '::' + class_name + '()\n{\n}\n')

    print 'generate class "' + class_name + '" succeed!'

	
def gen_null_file(file_name):
	str_header = get_pre_compile_header(file_name)
	
	_h = FileWriter(file_name + '.h','w')
	_cpp = FileWriter(file_name+'.cpp','w')
	
	_h.fprint('#ifndef ' + str_header)
	_h.fprint('#define ' + str_header)
	_h.fprint('#endif //'+ str_header)
	
	_cpp.fprint('#include "' +file_name+ '.h"')
	print 'generate null file "' + file_name + '" succeed!'
	
if __name__ == '__main__':

	count = len(argv)
	begin = 1
	type = 's'
	
	if count > 1 and argv[1] == '-h' :
		begin = 2
		type = 'h'
		
	for index in range(begin,count):
		if type == 'h':
			gen_null_file(argv[index])
		elif type == 's':
			gen_singlton_class(argv[index])