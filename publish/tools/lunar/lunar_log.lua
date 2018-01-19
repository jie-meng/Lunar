local ds = util.newUdpSocket(1)

local port = 9966
if ds:bind("", port) then
    print(string.format("Recv log on port %d start:\n", port))
    while true do
        local len = ds:recvFrom()
        if len > 0 then
            print(util.memToString(ds:getRecvBuf(ds)))
        else
            break
        end
    end    
end

ds:close()
ds:delete()

print(string.format("Recv log on port %d end.", port))
