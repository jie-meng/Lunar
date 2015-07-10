#coding=utf-8

import os
#import test.func
import xlrd
import test.mole
#from test.func import *
import test.func

book = xlrd.open_workbook('test.xlsx')
print "sheet count is ", book.nsheets

sh = book.sheet_by_index(0)

for rx in range (sh.nrows):
    print sh.row(rx)

print([d for d in os.listdir(".") if d.endswith("py")])

print(int("0xffff", 16))



print(dir(s))

def xlstest(a, b):
    pass
    
g = test.func.Student("s")




