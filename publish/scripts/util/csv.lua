Csv = {
    _csv = nil
}

function Csv:new(file)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    if file == "" then
        o._csv = csv.create()
    else
        o._csv = csv.create(file)
    end
    
    return o
end

function Csv:delete()
    csv.destroy(self._csv)
    _csv = nil
end

function Csv:read(file)
    return csv.read(self._csv, file)
end

function Csv:write(file)
    return csv.write(self._csv, file)
end

function Csv:empty()
    return csv.empty(self._csv)
end

function Csv:rows()
    return csv.rows(self._csv)
end

function Csv:cols()
    return csv.cols(self._csv)
end

function Csv:getCellValue(i, j)
    return csv.getCellValue(self._csv, i, j)
end

function Csv:setCellValue(i, j, value)
    return csv.setCellValue(self._csv, i, j, value)
end

function Csv:addRow(...)
    return csv.addRow(self._csv, ...)
end

function Csv:iterator()
    return CsvIterator:new(self)
end

CsvIterator = {
    
    i = 0,
    j = 0,
    _csv = nil
}

function CsvIterator:new(c)
    local o = {}
    setmetatable(o, self)
    self.__index = self
    
    o.i = 0
    o.j = 0
    o._csv = c
    
    return o
end

function CsvIterator:hasNext()
    if self.i+1 < self._csv:getTotalRows() then
        return true
    elseif self.i+1 > self._csv:getTotalRows() then
        return false
    else
       return self.j+1 < self._csv:getTotalCols() 
    end
end

function CsvIterator:next()
    
    if self.j+1 < self._csv:getTotalCols() then
        self.j = self.j + 1
    else
        self.i = self.i + 1
        self.j = 0
    end
    
    return self._csv:getCellValue(self.i, self.j), self.i, self.j
end

