class Milo:
    
    #def __init__(self, a):
    #    print "init Milo"
    
    def func1(self, s):
        print s
        
    def func2(self):
        print "func2"

class Cow:
    
    def __init__(self, a):
        print "init Cow"
        
    def test1(self, a, b):
        print a
        print b
    
    def test2(self):
        print "test2"

def shit():    
    print "shit"

class MiloExt(Milo, Cow):
    
    #def __init__(self, x):
    #    print "MiloExt init"
    
    def go(self, x, y):
        print "go"
