local hex_pattern = [[#(%w%w)(%w%w)(%w%w)]]
local rgba_pattern = [[rgb%s*%(%s*(%d+)%s*,%s*(%d+)%s*,%s*(%d+)%s*%)]]

print('hex -> rgb ? (y/n)')
local select = io.read()
if select == 'n' or select == 'N' then
    print('please input rgb values as "rgb(r, g, b)"')
    local input = io.read()
    local r, g, b = string.match(input, rgba_pattern)
    if r and g and b then
        print(string.format('#%02x%02x%02x', r, g, b))
    else
        print('parse failed')
    end
else    
    print('please input hex values as "#rrggbb"')
    local input = io.read()
    local r, g, b = string.match(input, hex_pattern)
    if r and g and b then
        print(string.format('rgb(%d, %d, %d)', tonumber(r, 16), tonumber(g, 16), tonumber(b, 16)))
    else
        print('parse failed')
    end
end
