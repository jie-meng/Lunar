Matrix = {
    _matrix = nil
}

function Matrix:new(row, col, type)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    if type == "" then
        o._matrix = matrix.create(row, col)
    else
        o._matrix = matrix.create(row, col, type)
    end
    
    return o
end

function Matrix:delete()
    matrix.destroy(self._matrix)
    _matrix = nil
end

function Matrix:toString()
    return matrix.toString(self._matrix)
end

function Matrix:rows()
    return matrix.rows(self._matrix)
end

function Matrix:cols()
    return matrix.cols(self._matrix)
end

function Matrix:get(i, j)
    return matrix.get(self._matrix, i, j)
end

function Matrix:set(i, j, value)
    matrix.set(self._matrix, i, j, value)
end

function Matrix:setElements(...)
    return matrix.setElements(self._matrix, ...)
end

function Matrix:empty()
    return matrix.empty(self._matrix)
end

function Matrix:square()
    return matrix.square(self._matrix)
end

function Matrix:elements()
    return matrix.elements(self._matrix)
end

function Matrix:rank()
    return matrix.rank(self._matrix)
end

function Matrix:determinant()
    return matrix.determinant(self._matrix)
end

function Matrix:invertable()
    return matrix.invertable(self._matrix)
end

function Matrix:inverse()
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.inverse(self._matrix)
    
    return o
end

function Matrix:transpose()
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.transpose(self._matrix)
    
    return o
end

function Matrix:conjugate()
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.conjugate(self._matrix)
    
    return o
end

function Matrix:adjoint()
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.adjoint(self._matrix)
    
    return o
end

function Matrix:addScalar(s)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.addScalar(self._matrix, s)
    
    return o
end

function Matrix:subtractScalar(s)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.subtractScalar(self._matrix, s)
    
    return o
end

function Matrix:multiplyScalar(s)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.multiplyScalar(self._matrix, s)
    
    return o
end

function Matrix:divideScalar(s)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.divideScalar(self._matrix, s)
    
    return o
end

function Matrix:add(m)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
        
    o._matrix = matrix.add(self._matrix, m._matrix)
    
    return o
end

function Matrix:subtract(m)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.subtract(self._matrix, m._matrix)
    
    return o
end

function Matrix:multiply(m)
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.multiply(self._matrix, m._matrix)
    
    return o
end

function Matrix:getRow()
    return matrix.getRow(self._matrix)
end

function Matrix:getCol()
    return matrix.getCol(self._matrix)
end

function Matrix:clone()
    
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o._matrix = matrix.clone(self._matrix)
    
    return o
end

function Matrix:print()
    print(matrix.toString(self._matrix))
end