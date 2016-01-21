import sys
import os
import re

regex_func = 'def\s+([\w_]+)\s*\((.*)\)\s*:'
regex_func_half = 'def\s+([\w_]+)\s*\('
regex_class = 'class\s+([\w_]+)\s*:'
regex_class_extend = 'class\s+([\w_]+)\s*\((.*)\)\s*:'

pattern_func = re.compile(regex_func)
pattern_func_half = re.compile(regex_func_half)
pattern_class = re.compile(regex_class)    
pattern_class_extend = re.compile(regex_class_extend)

class Function(object):
    def __init__(self, name, params, comment = ''):
        self.__name = name
        self.__params = params
        self.__comment = comment
        
    def getName(self):
        return self.__name
        
    def getParams(self):
        return self.__params
        
    def setParams(self, params):
        self.__params = params
        
    def getComment(self):
        return self.__comment

class Method(Function):
    def __init__(self, name, params, comment = ''):
        super(Method, self).__init__(name, params, comment)
        
        params = params.strip()
        if params.startswith('self'):
            params = params.replace('self', '', 1).lstrip()
            if params.startswith(','):
                params = params.replace(',', '', 1).lstrip()
        self.setParams(params)

class Cls(object):
    def __init__(self, indent, name, extends = []):
        self.__indent = indent
        self.__name = name
        self.__extends = extends
        self.__methods = []
        
    def getIndent(self):
        return self.__indent
    
    def getName(self):
        return self.__name
        
    def setName(self, name):
        self.__name = name
        
    def getExtends(self):
        return self.__extends
        
    def getMethods(self):
        return self.__methods
        
    def addMethod(self, method):
        self.__methods.append(method)

def getCurrentClsInStack(cls_stack):
    if len(cls_stack) > 0:
        return cls_stack[-1]
    else:
        return None
        
def getClsStackFullName(cls_stack):
    ret = ''
    if len(cls_stack) > 1:
        ret = '.'
        ret = ret.join([x.getName() for x in cls_stack])
        
    return ret
    
def trimInitOfPrefix(prefix):
    return prefix.replace('__init__.', '')

def parseLine(cls_stack, prefix, line, func_list, class_list):
    if len(line.strip()) == 0:
        return
    
    indent = len(line) - len(line.lstrip())
    cur_cls = getCurrentClsInStack(cls_stack)
    if cur_cls:
        if cur_cls.getIndent() == indent:
            stack_fullname = getClsStackFullName(cls_stack)
            if len(stack_fullname) > 0:
                cur_cls.setName(trimInitOfPrefix(prefix) + stack_fullname)
            else:
                cur_cls.setName(trimInitOfPrefix(prefix) + cur_cls.getName())
            class_list.append(cls_stack.pop())
    
    m = pattern_func.search(line)
    if m:
        if cur_cls:
            cur_cls.addMethod(Method(m.group(1), m.group(2)))
        else:
            func_list.append(Function(trimInitOfPrefix(prefix) + m.group(1), m.group(2)))
    else:
        m = pattern_func_half.search(line)
        if m:
            if cur_cls:
                cur_cls.addMethod(Method(m.group(1), m.group(2)))
            else:
                func_list.append(Function(trimInitOfPrefix(prefix) + m.group(1), '??'))
        else:
            m = pattern_class.search(line)
            if m:
                cls_stack.append(Cls(indent, m.group(1)))
            else:
                m = pattern_class_extend.search(line)
                if m:
                    cls_stack.append(Cls(indent, m.group(1), [x.strip() for x in m.group(2).split(',')]))
    
def parseFile(prefix, file, func_list, class_list):
    print("Parse file <%s>" % file)
    f = open(file, 'r')
    cls_stack = []
    try:
        lines = f.readlines()
        f.close()
        for line in lines:
            parseLine(cls_stack, prefix, line, func_list, class_list)
    except Exception as e:
        #print('Exception: %s' % e)
        pass
        
    while len(cls_stack) > 0:
        cls = getCurrentClsInStack(cls_stack)
        if cls:
            stack_fullname = getClsStackFullName(cls_stack)
            if len(stack_fullname) > 0:
                cls.setName(trimInitOfPrefix(prefix) + stack_fullname)
            else:
                cls.setName(trimInitOfPrefix(prefix) + cls.getName())
            
            class_list.append(cls_stack.pop())

def parseFolder(syspath_list, prefix, dir, func_list, class_list):
    #print('Parse folder [%s]' % dir)
    for x in os.listdir(dir):
        path = dir + '/' + x
        if os.path.isfile(path) and x.endswith('.py'):
            parseFile(prefix + x[:-2], path, func_list, class_list)
        elif os.path.isdir(path):
            if path not in syspath_list:
                if len([y for y in os.listdir(path) if y.endswith('py')]) > 0:
                    parseFolder(syspath_list, prefix + x + ".", path, func_list, class_list)

# main
sys_path_list = []
for path in sys.path:
    if os.getcwd() == path:
        continue
    
    if os.path.isdir(path):
        sys_path_list.append(path.replace('\\', '/'))

sys_path_list.sort(key = lambda x: len(x), reverse = True)

# parse
func_list = []
class_list = []
for sys_path in sys_path_list:
    parseFolder(sys_path_list, '', sys_path, func_list, class_list)
    
# saving
print('saving api file to \'python.api\' ...')
f = open('python.api', 'wt')

f.writelines([x + '\n' for x in dir(__builtins__) if not x.startswith('__')])
f.writelines(['string.' + x + '\n' for x in dir('') if not x.startswith('__')])
f.writelines(['list.' + x + '\n' for x in dir('list') if not x.startswith('__')])
f.writelines(['set.' + x + '\n' for x in dir('set') if not x.startswith('__')])
f.writelines(['dict.' + x + '\n' for x in dir('dict') if not x.startswith('__')])

f.writelines([x.getName() + '(' + x.getParams() + ')\n' for x in func_list])
for cls in class_list:
#    f.writelines([cls.getName() + '.' + x.getName() + '(' + x.getParams() + ')\n' for x in cls.getMethods()])
    for cls_method in cls.getMethods():
        f.write(cls.getName() + '.' + cls_method.getName() + '(' + cls_method.getParams() + ')\n')
        if cls_method.getName().strip() == "__init__":
            f.write(cls.getName() + '(' + cls_method.getParams() + ')\n')
            
f.close()

print('done')
